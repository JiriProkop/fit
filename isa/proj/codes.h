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

#endif
