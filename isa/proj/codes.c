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

#include "codes.h"

// This is used so I can ignore sprintf added '\0'
#define A_IgnoreOverflow           \
    _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wformat-overflow=\"")

#define A_Pop \
    _Pragma("GCC diagnostic pop")

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
A_IgnoreOverflow int send_ack(uint16_t block_num, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons((uint16_t)ack_opcode);
    block_num = htons(block_num);
    char *op = short_to_char(&op_code);
    char *block = short_to_char(&block_num);

    char msg[ACK_PACKET_SIZE];
    unsigned check = sprintf(msg, "%c%c%c%c", op[0], op[1], block[0], block[1]);
    assert(check == ACK_PACKET_SIZE); // FIXME remove before submiting
    return sendto(socket, msg, ACK_PACKET_SIZE, 0, (struct sockaddr *)&address, sizeof(address));
}
A_Pop

    /*
        @param err_msg Error message in netascii
    */
    int
    send_error(uint16_t err_code, char *err_msg, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons((uint16_t)error_opcode);
    err_code = htons(err_code);

    char *err = short_to_char(&err_code);
    char *op = short_to_char(&op_code);

    unsigned buff_size = sizeof(err_code) + sizeof(op_code) + strlen(err_msg) + 1; // strlen doesn't count the '\0'
    char msg[buff_size];
    unsigned check = sprintf(msg, "%c%c%c%c%s", op[0], op[1], err[0], err[1], err_msg);
    assert(check == buff_size); // FIXME remove before submiting
    return sendto(socket, msg, buff_size, 0, (struct sockaddr *)&address, sizeof(address));
}

int send_data(uint16_t block_num, char *data, unsigned data_len, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons((uint16_t)data_opcode);
    block_num = htons(block_num);

    char *op = short_to_char(&op_code);
    char *block = short_to_char(&block_num);

    // read or just give data, not decided yet
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
    unsigned check = sprintf(msg, "%c%c%s%c%s", op[0], op[1], filename, '\0', mode);
    assert(check == buff_size); // FIXME remove before submiting
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

/*
    @param num mode or block number or error code
*/
void print_info(struct sockaddr_in src_address, uint16_t opcode, uint16_t num, char *filepath) {
    // get source ip
    // get src port
    char *ip = inet_ntoa(src_address.sin_addr);
    int port = ntohs(src_address.sin_port);

    switch (opcode) {
        case read_req_opcode:
            fprintf(stderr, "RRQ {%s}:{%d} \"{%s}\" {%s} {}\n", ip, port, filepath, get_mode_name(num));
            // RRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
            break;
        case write_req_opcode:
            // WRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
            fprintf(stderr, "WRQ{%s}:{%d} \"{%s}\" {%s} {}\n", ip, port, filepath, get_mode_name(num));
            break;
        case ack_opcode:
            // ACK {SRC_IP}:{SRC_PORT} {BLOCK_ID}
            fprintf(stderr, "OACK\n");
            break;
        case data_opcode:
            // DATA {SRC_IP}:{SRC_PORT}:{DST_PORT} {BLOCK_ID}
            fprintf(stderr, "DATA\n");
            break;
        case error_opcode:
            fprintf(stderr, "ERROR\n");
            break;
        default:
            printf("Should never be here, something is terribly wrong!\n");
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
    // change CR LF to LF
    for (size_t i = 0; i < text_len; i++) {
        if (text[i] == '\r' && text[i + 1] == '\n') {
            text[i] = '\0';
        }
    }
}

// TODO add support for options
bool parse_req_packet(char *two_buf, char *filename, char *mode_str, char *msg, size_t msg_size) {
    if (msg_size < 4) {
        printf("Wrong packet format! \n");
        return false;
    }

    two_buf[0] = msg[0];
    two_buf[1] = msg[1];
    bool read_first_string = true;
    for (size_t i = 2; i < msg_size; i++) {
        if (read_first_string) {
            filename[0] = msg[i];
            filename++;
            if (msg[i] == '\0') {
                read_first_string = false;
            }
        } else {
            mode_str[0] = msg[i];
            mode_str++;
            if (msg[i] == '\0' && i + 1 != msg_size) {
                printf("Wrong packet format! \n");
                return false;
            }
        }
    }
    return true;
}

int create_socket_for_process() {
    int process_socket = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    if (process_socket < 0) {
        printf("Socket creation error!\n");
        return process_socket;
    }
    struct timeval tv;
    tv.tv_sec = SOCK_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(process_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        printf("Error setting timeout! \n");
        return -1; // TODO this should return something else than socket error, because here, err packet can be sent
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
    if (mode == octet_mode) {
        if (c == '\n') {
            buf[i++] = c;
        } else if (c == '\r') {
            buf[i++] = '\0';
        }
    }
    for (; i < n; i++) {
        c = fgetc(fp);
        if (mode == octet_mode && (c == '\r' || c == '\n')) {
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
