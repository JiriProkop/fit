#ifndef _TFTP_H
#define _TFTP_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdio.h>

#define ACK_PACKET_SIZE 4
#define RETRY_SENT_COUNT 3
#define SOCK_TIMEOUT 5

#define IP_PROTOCOL 0
#define MAX_PORT_NUM 65535
#define TFTP_DEFAULT_SERVER_PORT 69
#define TFTP_DEFAULT_DATA_SIZE 512 // FIXME this is true just for the data blocks, initial request can be longer

typedef enum {
    read_req_opcode = 1,
    write_req_opcode,
    data_opcode,
    ack_opcode,
    error_opcode,
    oack_opcode,
} packet_opcodes;

typedef enum {
    not_defined,
    file_not_found,
    access_violation,
    mem_error, // Disk full or allocation exceeded.
    illegal_operation,
    unknown_tid,
    file_already_exists,
    no_such_user,
    option_negotiation_error,
} packet_err_codes;

typedef enum {
    netascii_mode,
    octet_mode,
} modes;

typedef struct {
    bool blksize;
    size_t blksize_val;
    bool timeout;
    size_t timeout_val;
    bool tsize;
    off_t tsize_val;
} tftp_options_t; //TODO bool values are basicly useless

uint16_t short16_from_chars(char *buf);

inline char *short_to_char(uint16_t *short16) {
    return (char *)short16;
}

int send_ack(uint16_t block_num, struct sockaddr_in address, int socket);

int send_error(uint16_t err_code, char *err_msg, struct sockaddr_in address, int socket);

int send_data(uint16_t block_num, char *data, unsigned data_len, struct sockaddr_in address, int socket);

int send_request(uint16_t op_code, char *filename, char *mode, struct sockaddr_in address, int socket);

int parse_req_packet(char *two_buf, char *filename, char *mode_str, char *msg, size_t msg_size, tftp_options_t *options);

void text_from_mode(int mode, char *text, size_t text_len);

int create_socket_for_process(unsigned timeout);

size_t get_nchars_from_file(FILE *fp, size_t n, char *buf, int mode);

#endif
