typedef enum {
    read_req = 1,
    write_req,
    data,
    ack,
    error,
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
