/*
    Autor: Jiří Prokop, xproko47
    File: tftp.h
*/

#ifndef _TFTP_H
#define _TFTP_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#define ACK_PACKET_SIZE 4
#define RETRY_SENT_COUNT 4
#define SOCK_TIMEOUT 5
#define BLOCK_SIZE_MAX 65464

#define IP_PROTOCOL 0
#define MAX_PORT_NUM 65535
#define TFTP_DEFAULT_SERVER_PORT 69
#define TFTP_DEFAULT_DATA_SIZE 512

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
    char *blksize_name;

    bool timeout;
    size_t timeout_val;
    char *timeout_name;

    bool tsize;
    size_t tsize_val;
    char *tsize_name;

    char *order_names[4];
    size_t *order_vals[4];
} tftp_options_t;

uint16_t short16_from_chars(char *buf);

inline char *short_to_char(uint16_t *short16) { return (char *)short16; }

int send_ack(uint16_t block_num, struct sockaddr_in address, int socket);

int send_error(uint16_t err_code, char *err_msg, struct sockaddr_in address, int socket);

int send_data(uint16_t block_num, char *data, unsigned data_len, struct sockaddr_in address, int socket);

int send_request(uint16_t op_code, char *filename, char *mode, struct sockaddr_in address, int socket);

int send_0ack(struct sockaddr_in address, int socket, tftp_options_t *options);

int parse_req_packet(char *two_buf, char *filename, char *mode_str, char *msg, size_t msg_size, tftp_options_t *options);

void text_from_mode(int mode, char *text, size_t text_len);

bool set_socket_exp_timeout(int socket, int iter);

int create_socket_for_process(unsigned timeout);

size_t get_nchars_from_file(FILE *fp, size_t n, char *buf, int mode);

int get_port_from_socket(int socket);

void print_info_rrq(struct sockaddr_in src_address, uint16_t mode, char *filepath, tftp_options_t *options);

void print_info_wrq(struct sockaddr_in src_address, uint16_t mode, char *filepath, tftp_options_t *options);

void print_info_ack(struct sockaddr_in src_address, u_int16_t block_num);

void print_info_err(struct sockaddr_in src_address, uint16_t err_code, char *err_msg, int own_port);

void print_info_data(struct sockaddr_in src_address, u_int16_t block_num, int own_port);

void print_info_not_parsed(struct sockaddr_in src_address, char *msg, size_t msg_len, int own_socket);

#endif
