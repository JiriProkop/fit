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
#include <sys/types.h>
#include <unistd.h>

#include "codes.h"

// This is used so I can ignore sprintf added '\0'
#define A_IgnoreOverflow           \
    _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wformat-overflow=\"")

#define A_Pop \
    _Pragma("GCC diagnostic pop")

uint16_t opcode_from_chars(char buf[2]) {
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
static inline char *short_to_char(uint16_t *short16) {
    return (char *)short16;
}

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
    unsigned check = sprintf(msg, "%c%c%c%c%*s", op[0], op[1], block[0], block[1], data_len, data);
    assert(check == buff_size); // FIXME remove before submiting
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

/*
    @param num mode or block number
*/
void print_info(struct sockaddr_in adress, uint16_t opcode, uint16_t num, char *filepath) {
    // get source ip
    // get src port

    switch (opcode) {
        case read_req_opcode:
            fprintf(stderr, "RRQ\n");
            // RRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
            break;
        case write_req_opcode:
            // WRQ {SRC_IP}:{SRC_PORT} "{FILEPATH}" {MODE} {$OPTS}
            fprintf(stderr, "WRQ\n");
            break;
        case ack_opcode:
            // ACK {SRC_IP}:{SRC_PORT} {BLOCK_ID}
            fprintf(stderr, "OACK\n");
            break;
        case data_opcode:
            fprintf(stderr, "DATA\n");
            break;
        case error_opcode:
            fprintf(stderr, "ERROR\n");
            break;
        default:
            printf("Should never be here, something is terribly wrong!\n");
            break;
    }

    (void)adress;
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
    for (int i = 0; i < *text_len; i++) {
        tmp[i + added_count] = text[i];
        if (text[i] == '\n' || text[i] == '\r') {
            tmp[i + added_count] = '\r';
            char *tmp_real = realloc(tmp, text_len + ++added_count);
            if (tmp_real == NULL) {
                printf("Malloc error! \n");
                return NULL;
            }
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
        return text;
    }
    // change CR LF to LF
    for (int i = 0; i < text_len; i++) {
        if (text[i] == '\r' && text[i + 1] == '\n') {
            text[i] = '\0';
        }
    }
}

// int main() {
//     uint16_t op_code = 1024;
//     uint16_t block_num = 1;

//     char *op = short_to_char(&op_code);
//     char *block = short_to_char(&block_num);

//     printf("%d %d - %d %d \n", op[0], op[1], block[0], block[1]);

//     return 0;
// }
