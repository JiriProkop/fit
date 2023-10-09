#ifndef _TFTP_H
#define _TFTP_H

#define ACK_PACKET_SIZE 4

typedef enum {
    read_req_opcode = 1, // FIXME add _opcode or smth to the end
    write_req_opcode,
    data_opcode,
    ack_opcode,
    error_opcode,
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
} packet_err_codes;

uint16_t opcode_from_chars(char buf[2]);

int send_ack(uint16_t block_num, struct sockaddr_in address, int socket);

int send_error(uint16_t err_code, char *err_msg, struct sockaddr_in address, int socket);

int send_data(uint16_t block_num, char *data, unsigned data_len, struct sockaddr_in address, int socket);

int send_request(uint16_t op_code, char *filename, char *mode, struct sockaddr_in address, int socket);

#endif
