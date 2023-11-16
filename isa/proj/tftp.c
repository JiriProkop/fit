#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "tftp.h"

uint16_t short16_from_chars(char *buf) {
    union {
        char ch[2];
        uint16_t n;
    } char2short;
    char2short.ch[0] = buf[0];
    char2short.ch[1] = buf[1];
    return char2short.n;
}

/*
    Just changes the goggles with  which compiler looks at the data
*/
extern inline char *short_to_char(uint16_t *short16);

/*
    @return Returns int < 0 if an error occured
*/
int send_ack(uint16_t block_num, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons((uint16_t)ack_opcode);
    block_num = htons(block_num);
    char *op = short_to_char(&op_code);
    char *block = short_to_char(&block_num);

    char msg[ACK_PACKET_SIZE];
    msg[0] = op[0];
    msg[1] = op[1];
    msg[2] = block[0];
    msg[3] = block[1];
    return sendto(socket, msg, ACK_PACKET_SIZE, 0, (struct sockaddr *)&address, sizeof(address));
}

/*
    @param err_msg Error message in netascii
*/
int send_error(uint16_t err_code, char *err_msg, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons((uint16_t)error_opcode);
    err_code = htons(err_code);

    char *err = short_to_char(&err_code);
    char *op = short_to_char(&op_code);

    unsigned buff_size = sizeof(err_code) + sizeof(op_code) + strlen(err_msg) + 1; // strlen doesn't count the '\0'
    char msg[buff_size];
    unsigned check = sprintf(msg, "%c%c%c%c%s", op[0], op[1], err[0], err[1], err_msg) + 1;
    assert(check == buff_size); // FIXME remove before submiting
    return sendto(socket, msg, buff_size, 0, (struct sockaddr *)&address, sizeof(address));
}

int send_data(uint16_t block_num, char *data, unsigned data_len, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons((uint16_t)data_opcode);
    block_num = htons(block_num);

    char *op = short_to_char(&op_code);
    char *block = short_to_char(&block_num);

    unsigned buff_size = data_len + sizeof(op_code) + sizeof(block_num);
    char msg[buff_size];
    msg[0] = op[0];
    msg[1] = op[1];
    msg[2] = block[0];
    msg[3] = block[1];
    size_t i;
    for (i = 4; i < data_len + 4; i++) {
        msg[i] = data[i - 4];
    }
    assert(i == buff_size); // FIXME remove before submiting
    return sendto(socket, msg, buff_size, 0, (struct sockaddr *)&address, sizeof(address));
}

int send_request(uint16_t op_code, char *filename, char *mode, struct sockaddr_in address, int socket) {
    op_code = htons(op_code);
    char *op = short_to_char(&op_code);

    unsigned buff_size = strlen(filename) + 1 + strlen(mode) + 1 + sizeof(op_code);
    char msg[buff_size];
    unsigned check = sprintf(msg, "%c%c%s%c%s", op[0], op[1], filename, '\0', mode) + 1; // +1 for the '\0' at the end
    assert(check == buff_size);                                                          // FIXME remove before submiting
    return sendto(socket, msg, buff_size, 0, (struct sockaddr *)&address, sizeof(address));
}

/*
    Including the '\0'
*/
int get_char_size_of_num(size_t num) {
    int i = 0;
    for (; num != 0; i++) {
        num /= 10;
    }
    return i + 1;
}

long get_0ack_buff_size(tftp_options_t *options) {
    long buff_size = 2;
    if (options->blksize) {
        buff_size += strlen("blksize") + 1;
        buff_size += get_char_size_of_num(options->blksize_val);
    }
    if (options->timeout) {
        buff_size += strlen("timeout") + 1;
        buff_size += get_char_size_of_num(options->timeout_val);
    }
    if (options->tsize) {
        buff_size += strlen("tsize") + 1;
        buff_size += get_char_size_of_num(options->tsize_val);
    }
    return buff_size;
}

int send_0ack(struct sockaddr_in address, int socket, tftp_options_t *options) {
    uint16_t op_code = htons((uint16_t)oack_opcode);
    char *op = short_to_char(&op_code);
    long buff_size = get_0ack_buff_size(options);

    char msg[buff_size];
    msg[0] = op[0];
    msg[1] = op[1];
    char *msg_ptr = msg + 2;

    if (options->blksize) {
        msg_ptr += sprintf(msg_ptr, "%s%c%zu", "blksize", '\0', options->blksize_val) + 1;
    }
    if (options->timeout) {
        msg_ptr += sprintf(msg_ptr, "%s%c%zu", "timeout", '\0', options->timeout_val) + 1;
    }
    if (options->tsize) {
        msg_ptr += sprintf(msg_ptr, "%s%c%zu", "tsize", '\0', options->tsize_val) + 1;
    }

    assert(msg_ptr - msg == buff_size); // FIXME remove before submiting
    return sendto(socket, msg, buff_size, 0, (struct sockaddr *)&address, sizeof(address));
}

char *get_mode_name(uint16_t mode) {
    if (mode == netascii_mode) {
        return "netascii";
    } else if (mode == octet_mode) {
        return "octet";
    }
    return "";
}

int get_port_from_socket(int socket) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socket, (struct sockaddr *)&sin, &len) == -1) {
        printf("getsockname error - cannot get own port number! \n");
        return -1;
    } else {
        return ntohs(sin.sin_port);
    }
}

void print_info_rrq(struct sockaddr_in src_address, uint16_t mode, char *filepath, tftp_options_t *options) {
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    // RRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
    fprintf(stderr, "RRQ %s:%d \"%s\" %s", ip, port, filepath, get_mode_name(mode));
    for (int i = 0; options->order_names[i] != NULL; i++) {
        fprintf(stderr, " %s=%zu", options->order_names[i], *(options->order_vals[i]));
    }
    fprintf(stderr, "\n");
}

void print_info_wrq(struct sockaddr_in src_address, uint16_t mode, char *filepath, tftp_options_t *options) {
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    // WRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
    fprintf(stderr, "WRQ %s:%d \"%s\" %s", ip, port, filepath, get_mode_name(mode));
    for (int i = 0; options->order_names[i] != NULL; i++) {
        fprintf(stderr, " %s=%zu", options->order_names[i], *(options->order_vals[i]));
    }
    fprintf(stderr, "\n");
}

void print_info_ack(struct sockaddr_in src_address, u_int16_t block_num) {
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    // ACK {SRC_IP}:{SRC_PORT} {BLOCK_ID}
    fprintf(stderr, "ACK %s:%d %d\n", ip, port, block_num);
}

void print_info_err(struct sockaddr_in src_address, uint16_t err_code, char *err_msg, int own_port) {
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    // ERROR {SRC_IP}:{SRC_PORT}:{DST_PORT} {CODE} "{MESSAGE}"
    fprintf(stderr, "ERROR %s:%d:%d %d \"%s\"\n", ip, port, own_port, err_code, err_msg);
}

void print_info_data(struct sockaddr_in src_address, u_int16_t block_num, int own_port) {
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    // DATA {SRC_IP}:{SRC_PORT}:{DST_PORT} {BLOCK_ID}
    fprintf(stderr, "DATA %s:%d:%d %d\n", ip, port, own_port, block_num);
}

void print_unparsed_options(char *msg, size_t msg_len) {
    unsigned word_count = 0;
    char *start = msg;
    for (size_t i = 0; i < msg_len; i++) {
        if (msg[i] == '\0') {
            if (word_count % 2 == 0) {
                fprintf(stderr, " %s=", start);
            } else {
                fprintf(stderr, "%s", start);
            }

            start = msg + i + 1;
            word_count++;
        }
    }
    fprintf(stderr, "\n");
}

void print_info_not_parsed(struct sockaddr_in src_address, char *msg, size_t msg_len, int own_socket) {
    if (msg_len < 4)
        return;
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    uint16_t code = short16_from_chars(msg);
    code = ntohs(code);
    uint16_t packet_block_num;
    unsigned word_count;
    size_t i;
    char *start;
    uint16_t err_code;

    switch (code) {
        case oack_opcode:
            // OACK {SRC_IP}:{SRC_PORT} {$OPTS}
            fprintf(stderr, "OACK %s:%d", ip, port);
            print_unparsed_options(msg + 2, msg_len - 2);
            break;

        case ack_opcode:
            packet_block_num = short16_from_chars(msg + 2);
            packet_block_num = ntohs(packet_block_num);

            // ACK {SRC_IP}:{SRC_PORT} {BLOCK_ID}
            fprintf(stderr, "ACK %s:%d %d\n", ip, port, packet_block_num);
            break;

        case data_opcode:
            packet_block_num = short16_from_chars(msg + 2);
            packet_block_num = ntohs(packet_block_num);

            // DATA {SRC_IP}:{SRC_PORT}:{DST_PORT} {BLOCK_ID}
            fprintf(stderr, "DATA %s:%d:%d %d\n", ip, port, get_port_from_socket(own_socket), packet_block_num);
            break;

        case error_opcode:
            err_code = short16_from_chars(msg + 2);
            err_code = ntohs(err_code);

            // ERROR {SRC_IP}:{SRC_PORT}:{DST_PORT} {CODE} "{MESSAGE}"
            fprintf(stderr, "ERROR %s:%d:%d %d", ip, port, get_port_from_socket(own_socket), err_code);
            msg += 4;
            for (i = 0; i < msg_len - 4 && msg[i] != '\0'; i++) {
                fputc(msg[i], stderr);
            }
            fputc('\n', stderr);
            break;

        case read_req_opcode:
            // RRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
            fprintf(stderr, "RRQ %s:%d", ip, port);

            word_count = 0;
            start = msg + 2;
            for (i = 2; i < msg_len && word_count < 2; i++) {
                if (msg[i] == '\0') {
                    fprintf(stderr, " %s", start);
                    start = msg + i + 1;
                    word_count++;
                }
            }
            print_unparsed_options(start, msg_len - i);
            break;

        case write_req_opcode:
            // WRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
            fprintf(stderr, "WRQ %s:%d", ip, port);

            word_count = 0;
            start = msg + 2;
            for (i = 2; i < msg_len && word_count < 2; i++) {
                if (msg[i] == '\0') {
                    fprintf(stderr, " %s", start);
                    start = msg + i + 1;
                    word_count++;
                }
            }
            print_unparsed_options(start, msg_len - i);
            break;
    }
}

/*
    Converts text from ascii to mode. In text_len is returned the new size
*/
char *text_to_mode(int mode, char *text, size_t *text_len) {
    if (mode == octet_mode) {
        return text;
    }
    char *tmp = malloc(*text_len);
    if (tmp == NULL) {
        printf("Malloc error! \n");
        return NULL;
    }
    // change each LF to CR LF
    int added_count = 0;
    for (size_t i = 0; i < *text_len; i++) {
        tmp[i + added_count] = text[i];
        if (text[i] == '\n' || text[i] == '\r') {
            tmp[i + added_count] = '\r';
            char *tmp_real = realloc(tmp, (*text_len) + ++added_count);
            if (tmp_real == NULL) {
                printf("Malloc error! \n");
                return NULL;
            }
            tmp = tmp_real;
            tmp[i + added_count] = text[i] == '\n' ? '\n' : '\0';
        }
    }
    // while this if isn't technicaly needed, it returns the original string if there
    //  was no need to change it, which will reduce damage caused by this fn's error/s
    if (added_count == 0) {
        free(tmp);
        return text;
    } else {
        free(text);
        *text_len = *text_len + added_count;
        return tmp;
    }
}

/*
    Change NETASCII to ASCII if needed.
*/
void text_from_mode(int mode, char *text, size_t text_len) {
    if (mode == octet_mode) {
        return;
    }
    static bool hanging_cr = false;

    // change CR LF to LF
    for (size_t i = 0; i < text_len; i++) {
        if (hanging_cr && text[0] == '\0') {
            text[0] = '\r';
            hanging_cr = false;
            continue;
        }
        if (text[i] == 'r' && i + 1 == text_len) {
            hanging_cr = true;
            continue;
        } else if (text[i] == '\r' && text[i + 1] == '\n') {
            text[i] = '\0';
        }
    }
}

char blksize_str[] = "blksize";
char timeout_str[] = "timeout";
char tsize_str[] = "tsize";

void init_options(tftp_options_t *options) {
    options->blksize = false;
    options->blksize_val = 0;
    options->blksize_name = blksize_str;

    options->timeout = false;
    options->timeout_val = 0;
    options->timeout_name = timeout_str;

    options->tsize = false;
    options->tsize_val = 0;
    options->tsize_name = tsize_str;
}

bool missing_option_value(tftp_options_t *options) {
    if (options->blksize && options->blksize_val == 0) {
        printf("Missing blksize option value!! \n");
        return true;
    } else if (options->timeout && options->timeout_val == 0) {
        printf("Missing timeout option value!! \n");
        return true;
    } else if (options->tsize && options->tsize_val == 0) {
        printf("Missing tsize option value!! \n");
        return true;
    }
    return false;
}

typedef enum {
    read_file_name,
    read_mode,
    read_option,
    read_option_value,
} state_enum;

typedef enum {
    none,
    blksize,
    timeout,
    tsize,
} value_for_enum;

int parse_req_packet(char *two_buf, char *filename, char *mode_str, char *msg, size_t msg_size, tftp_options_t *options) {
    if (msg_size < 4) {
        printf("Wrong packet format! \n");
        return 1;
    }
    two_buf[0] = msg[0];
    two_buf[1] = msg[1];

    init_options(options);
    int temp_buf_size = 32;
    char temp[temp_buf_size];
    int temp_i = 0;
    int state = read_file_name;
    int value_for = none;
    int option_counter = 0;
    size_t i;
    for (i = 2; i < msg_size; i++) {
        if (temp_i >= temp_buf_size) {
            printf("Option name or value too long(bigger than %d chars)! \n", temp_buf_size);
            return 2;
        }
        switch (state) {
            case read_file_name:
                filename[temp_i++] = msg[i];
                if (msg[i] == '\0') {
                    temp_i = 0;
                    state = read_mode;
                }
                break;

            case read_mode:
                mode_str[temp_i++] = tolower(msg[i]);
                if (msg[i] == '\0') {
                    temp_i = 0;
                    state = read_option;
                }
                break;

            case read_option:
                temp[temp_i++] = tolower(msg[i]);
                if (msg[i] == '\0') {
                    temp_i = 0;
                    if (strcmp(temp, "blksize") == 0) {
                        options->blksize = true;
                        value_for = blksize;
                        options->order_names[option_counter] = options->blksize_name;
                        options->order_vals[option_counter++] = &(options->blksize_val);
                    } else if (strcmp(temp, "timeout") == 0) {
                        value_for = timeout;
                        options->timeout = true;
                        options->order_names[option_counter] = options->timeout_name;
                        options->order_vals[option_counter++] = &(options->timeout_val);
                    } else if (strcmp(temp, "tsize") == 0) {
                        value_for = tsize;
                        options->tsize = true;
                        options->order_names[option_counter] = options->tsize_name;
                        options->order_vals[option_counter++] = &(options->tsize_val);
                    } else {
                        value_for = none;
                    }
                    state = read_option_value;
                }
                break;

            case read_option_value:
                if (msg[i] == '\0') {
                    temp_i = 0;
                    if (value_for == timeout) {
                        options->timeout_val = atoi(temp);
                        if (!(options->timeout_val >= 1 && options->timeout_val <= 255)) {
                            printf("Invalid timeout option value!! \n");
                            return 2;
                        }
                    } else if (value_for == blksize) {
                        options->blksize_val = atoi(temp);
                        if (!(options->blksize_val >= 8)) {
                            printf("Invalid blocksize option value!! \n");
                            return 2;
                        } else if (options->blksize_val > BLOCK_SIZE_MAX) {
                            options->blksize_val = BLOCK_SIZE_MAX;
                        }
                    } else if (value_for == tsize) {
                        options->tsize_val = atoi(temp);
                        if (options->tsize_val == 0) {
                            printf("Invalid tsize option value!! \n");
                            return 2;
                        }
                    }
                    state = read_option;
                } else if (value_for == none) {
                    break;
                } else if (isdigit(msg[i])) {
                    temp[temp_i++] = msg[i];
                } else {
                    printf("Invalid option value!! \n");
                    return 2;
                }
                break;
        }
    }
    options->order_names[option_counter] = NULL;
    if (missing_option_value(options)) {
        return 2;
    }
    if (msg[i - 1] != '\0') {
        printf("Wrong packet format!\n");
        return 1;
    }
    return 0;
}

bool set_socket_exp_timeout(int socket, int iter) {
    struct timeval tv;
    unsigned timeout = SOCK_TIMEOUT;
    for (int i = 0; i < iter; i++) {
        timeout *= 2;
    }
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        printf("Error setting timeout! \n");
        return false;
    }
    return true;
}

int create_socket_for_process(unsigned timeout) {
    int process_socket = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    if (process_socket < 0) {
        printf("Socket creation error!\n");
        return process_socket;
    }
    struct timeval tv;
    tv.tv_sec = timeout ? timeout : SOCK_TIMEOUT;
    tv.tv_usec = 0;

    if (setsockopt(process_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        printf("Error setting timeout! \n");
        close(process_socket);
        return -1;
    }
    return process_socket;
}

/*
    buf is expected to be already allocated to n size

    @return returns number of read chars
*/
size_t get_nchars_from_file(FILE *fp, size_t n, char *buf, int mode) {
    static int c = 0;
    size_t i = 0;
    // put left over letter from last reading
    if (mode == netascii_mode) {
        if (c == '\n') {
            buf[i++] = c;
        } else if (c == '\r') {
            buf[i++] = '\0';
        }
    }
    for (; i < n; i++) {
        c = fgetc(fp);
        if (mode == netascii_mode && (c == '\r' || c == '\n')) {
            buf[i] = '\r';
            if (++i == n) {
                return i;
            }
            if (c == '\r') {
                c = '\0';
            }
        }
        if (c == EOF) {
            return i;
        }
        buf[i] = c;
    }
    return i;
}
