typedef enum {
    read_req = 1,
    write_req = 2,
    data = 3,
    ack = 4,
    error = 5,
} packet_opcodes;

typedef enum {
    not_defined = 0,
    file_not_found = 1,
    access_violation = 2,
    mem_error = 3, // Disk full or allocation exceeded.
    illegal_operation = 4,
    unknown_tid = 5,
    file_already_exists = 6,
    no_such_user = 7,
} packet_err_codes;
