#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tftp.h"

typedef struct {
    long port;
    char *root_path;
} args_t;

args_t args = {.root_path = NULL, .port = TFTP_DEFAULT_SERVER_PORT};
int server_socket;
bool close_socket = false;

/*
    Free allocated resources and exit.

    @param failure true for EXIT_FAILURE return code, EXIT_SUCCESS otherwise
*/
void free_and_exit(bool failure) {
    free(args.root_path);
    if (close_socket) {
        close(server_socket);
    }

    if (failure) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}

void close_socket_and_exit(bool failure, int socket) {
    close(socket);
    if (failure) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}

void print_help() {
    printf("Usage: tftp-server [-p port] root_dirpath\n");
    printf("  -h, --help\tPrint this help message\n");
    printf("  -p\t\tLocal port on which the server will expect incoming connections\n");
    printf("    \t\tDefault port is 69 if not specified\n");
    printf("root_dirpath\tPath to the directory where incoming files will be saved\n");
}

void parse_args(int argc, char *argv[]) {
    if (argc == 2) {
        if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
            print_help();
            exit(EXIT_SUCCESS);
        }
        args.root_path = malloc(strlen(argv[1]) + 1);
        if (args.root_path == NULL) {
            printf("Memory allocation error!\n");
            exit(EXIT_FAILURE);
        }
        strcpy(args.root_path, argv[1]);
    } else if (argc == 4) {
        if (!strcmp(argv[1], "-p")) {
            char *check;
            args.port = strtoul(argv[2], &check, 10);
            if (args.port < 0 || args.port > MAX_PORT_NUM) {
                printf("Invalid socket number! (must be between 0 and %d)\n", MAX_PORT_NUM + 1);
                exit(EXIT_FAILURE);
            } else if (args.port == 0) {
                printf("Invalid socket arg value! (must be integer > 0)\n");
                exit(EXIT_FAILURE);
            }

            args.root_path = malloc(strlen(argv[3]) + 1);
            if (args.root_path == NULL) {
                printf("Memory allocation error!\n");
                exit(EXIT_FAILURE);
            }
            strcpy(args.root_path, argv[3]);
        } else {
            printf("Wrong argument name '-p' or it's position!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Wrong argument count!\n");
        print_help();
        exit(EXIT_FAILURE);
    }
    // check if dir exists
    DIR *dir = opendir(args.root_path);
    if (dir) {
        closedir(dir); // Directory exists.
    } else if (errno == ENOENT) {
        // Directory does not exist.
        printf("Dir %s does not exist! \n", args.root_path);
        free_and_exit(true);
    } else {
        // opendir() failed for some other reason.
        printf("Opendir failed for some reason on dir %s! \n", args.root_path);
        free_and_exit(true);
    }
}

void sig_handler(int _) {
    (void)_;
    printf("\nSIGINT received, clearing allocations.\n");
    free_and_exit(false);
}

void set_tsize_value_ifset(FILE *fp, tftp_options_t *options) {
    if (options->tsize) {
        struct stat st;
        if (fstat(fp->_fileno, &st) != 0) {
            printf("Couldn't get file size! %d \n Going without tsize option. \n", errno);
            options->tsize = false;
        } else {
            options->tsize_val = st.st_size;
        }
    }
}

void tftp_read(struct sockaddr_in client_address, int mode, const char file_path[], tftp_options_t options) {
    size_t max_data_size = options.blksize ? options.blksize_val : TFTP_DEFAULT_DATA_SIZE;
    size_t buff_size = max_data_size < TFTP_DEFAULT_DATA_SIZE ? TFTP_DEFAULT_DATA_SIZE + 4 : max_data_size + 4;
    char buf[buff_size];
    int process_socket = create_socket_for_process(options.timeout_val);
    if (process_socket < 0) {
        send_error(not_defined, "Socket creation failed!", client_address, server_socket);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Couldn't read the file! (not present or insufficient privileges) %d \n", errno);
        send_error(file_not_found, "Couldn't read the file! (not present or insufficient privileges)!", client_address, process_socket);
        close_socket_and_exit(true, process_socket);
    }
    set_tsize_value_ifset(fp, &options);

    int bytestx;
    socklen_t addr_len = sizeof(client_address);
    bool to_break = false;
    int prev_client_port = client_address.sin_port;
    struct in_addr prev_ip = client_address.sin_addr;
    uint16_t block_num = 1;
    size_t data_size;
    bool first_packet = true;
    bool any_option = options.blksize || options.timeout || options.tsize;

    while (1) {
    sending_data_packet_read:
        if (!(first_packet && any_option)) {
            data_size = get_nchars_from_file(fp, max_data_size, buf, mode);
            printf("sending data of size: %ld \n", data_size);
            to_break = data_size < max_data_size;
        }
        for (int i = 0; i < RETRY_SENT_COUNT; i++) {
            if (!options.timeout && !set_socket_exp_timeout(process_socket, i)) { // exponential timeout increase
                send_error(not_defined, "Error setting timeout!", client_address, process_socket);
                fclose(fp);
                close_socket_and_exit(true, process_socket);
            }
            if (!(first_packet && any_option)) {
                bytestx = send_data(block_num, buf, data_size, client_address, process_socket);
            } else {
                bytestx = send_0ack(client_address, process_socket, &options);
                block_num = 0;
            }
            if (bytestx < 0) {
                printf("Sendto error! \n");
                continue;
            }
            // wait for ack
            bytestx = recvfrom(process_socket, buf, TFTP_DEFAULT_DATA_SIZE + 4, 0, (struct sockaddr *)&client_address, &addr_len);
            if (bytestx < 0) {
                printf("Recvfrom error or timeout! \n");
                continue;
            }
            if (client_address.sin_addr.s_addr != prev_ip.s_addr || client_address.sin_port != prev_client_port) {
                print_info_not_parsed(client_address, buf, bytestx, process_socket);
                send_error(unknown_tid, "Unknown sender address or TID!", client_address, process_socket);
                client_address.sin_addr = prev_ip;
                client_address.sin_port = prev_client_port;
                continue;
            }
            uint16_t packet_block_num;
            // check ack, print info
            uint16_t code = short16_from_chars(buf);
            code = ntohs(code);
            switch (code) {
                case error_opcode:
                    uint16_t err_code = short16_from_chars(buf + 2);
                    err_code = ntohs(code);
                    print_info_err(client_address, err_code, buf + 4, get_port_from_socket(process_socket));
                    fclose(fp);
                    close_socket_and_exit(true, process_socket);
                    break;

                case ack_opcode:
                    packet_block_num = ntohs(short16_from_chars(buf + 2));
                    print_info_ack(client_address, packet_block_num);
                    if (block_num != packet_block_num) {
                        continue;
                    } else {
                        if (to_break) {
                            goto end_loop_read; // this was the last packet, we can leave
                        }
                        first_packet = false;
                        block_num++;
                        goto sending_data_packet_read;
                    }
                    break;
                case oack_opcode:
                case data_opcode:
                case read_req_opcode:
                case write_req_opcode:
                    print_info_not_parsed(client_address, buf, bytestx, process_socket);
                    break;
                default:
                    printf("Received packet has unknown opcode! \n");
                    fclose(fp);
                    close_socket_and_exit(true, process_socket);
            }
        }
        // didnt get ack for the RETRY_SENT_COUNTth time -> quiting
        send_error(not_defined, "Timed out!", client_address, process_socket);
        fclose(fp);
        close_socket_and_exit(true, process_socket);
    }
end_loop_read:
    printf("READINF OF %s DONE \n", file_path);
    fclose(fp);
    close_socket_and_exit(false, process_socket);
}

bool file_already_exists_check(const char file_path[]) {
    if (access(file_path, F_OK) == 0) {
        printf("file already exists \n");
        return true;
    }
    return false;
}

void tftp_write(struct sockaddr_in client_address, int mode, const char file_path[], tftp_options_t options) {
    size_t max_data_size = options.blksize ? options.blksize_val : TFTP_DEFAULT_DATA_SIZE;
    size_t buff_size = max_data_size < TFTP_DEFAULT_DATA_SIZE ? TFTP_DEFAULT_DATA_SIZE + 4 : max_data_size + 4;
    char buf[buff_size];
    int process_socket = create_socket_for_process(options.timeout_val);
    if (process_socket < 0) {
        send_error(not_defined, "Socket creation error!", client_address, server_socket);
        exit(EXIT_FAILURE);
    }
    if (file_already_exists_check(file_path)) {
        send_error(file_already_exists, "File already exists!", client_address, process_socket);
        close_socket_and_exit(true, process_socket);
    }

    FILE *fp = fopen(file_path, "w");
    if (fp == NULL) {
        printf("Couldn't create the file! (probably insufficient privileges) %d \n", errno);
        send_error(access_violation, "Couldn't create the file!", client_address, process_socket);
        close_socket_and_exit(true, process_socket);
    }
    int bytestx;
    socklen_t addr_len = sizeof(client_address);
    bool to_break = false;
    int prev_client_port = client_address.sin_port;
    struct in_addr prev_ip = client_address.sin_addr;
    uint16_t block_num = 0;
    bool first_packet = true;
    bool any_option = options.blksize || options.timeout || options.tsize;

    while (1) {
    sending_ack_packet_write:
        for (int i = 0; i < RETRY_SENT_COUNT; i++) {
            if (!options.timeout && !set_socket_exp_timeout(process_socket, i)) {
                send_error(not_defined, "Error setting timeout!", client_address, process_socket);
                fclose(fp);
                close_socket_and_exit(true, process_socket);
            }
            // send ack
            if (first_packet && any_option) {
                if (send_0ack(client_address, process_socket, &options) < 0) {
                    printf("Sendto error! \n");
                    continue;
                }
                block_num++;
            } else if (send_ack(block_num, client_address, process_socket) < 0) {
                printf("Sendto error! \n");
                continue;
            }
            if (to_break) {
                goto end_loop_write;
            }

            bytestx = recvfrom(process_socket, buf, buff_size, 0, (struct sockaddr *)&client_address, &addr_len);
            if (bytestx < 0) {
                printf("Recvfrom error or timeout! \n");
                continue;
            }
            if (client_address.sin_addr.s_addr != prev_ip.s_addr || client_address.sin_port != prev_client_port) {
                print_info_not_parsed(client_address, buf, bytestx, process_socket);
                send_error(unknown_tid, "Unknown sender address or TID!", client_address, process_socket);
                client_address.sin_addr = prev_ip;
                client_address.sin_port = prev_client_port;
                continue;
            }
            size_t data_size = bytestx - 4;
            // get data, print info
            uint16_t code = short16_from_chars(buf);
            code = ntohs(code);
            switch (code) {
                case error_opcode:
                    uint16_t err_code = short16_from_chars(buf + 2);
                    err_code = ntohs(code);
                    print_info_err(client_address, err_code, buf + 4, get_port_from_socket(process_socket));

                    fclose(fp);
                    close_socket_and_exit(true, process_socket);
                    break;
                case data_opcode:
                    uint16_t packet_block_num = ntohs(short16_from_chars(buf + 2));
                    print_info_data(client_address, packet_block_num, get_port_from_socket(process_socket));

                    if (block_num + 1 != packet_block_num) {
                        continue;
                    }
                    to_break = data_size < max_data_size;
                    first_packet = false;

                    printf("writing file... \n");
                    text_from_mode(mode, buf + 4, data_size); // convert from netascii to normalascii
                    // write
                    for (size_t j = 0; j < data_size; j++) {
                        fputc(buf[4 + j], fp);
                    }
                    block_num++;
                    goto sending_ack_packet_write;
                case oack_opcode:
                case ack_opcode:
                case read_req_opcode:
                case write_req_opcode:
                    print_info_not_parsed(client_address, buf, bytestx, process_socket);
                    break;
                default:
                    printf("Received packet has unknown opcode! \n");
                    fclose(fp);
                    close_socket_and_exit(true, process_socket);
            }
        }
        // didnt get ack for the RETRY_SENT_COUNTth time -> quiting
        send_error(not_defined, "Timed out!", client_address, process_socket);
        fclose(fp);
        close_socket_and_exit(true, process_socket);
    }
end_loop_write:
    printf("WRITING OF %s DONE \n", file_path);
    fclose(fp);
    close_socket_and_exit(false, process_socket);
}

void main_loop() {
    struct sockaddr_in server_addres;
    server_addres.sin_family = AF_INET;
    server_addres.sin_port = htons(args.port);
    server_addres.sin_addr.s_addr = INADDR_ANY;

    server_socket = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    if (server_socket < 0) {
        printf("Socket creation error!\n");
        free_and_exit(true);
    }

    if (bind(server_socket, (struct sockaddr *)&server_addres, sizeof(server_addres)) < 0) {
        printf("Socket binding failed!\n");
        free_and_exit(true);
    } else {
        close_socket = true;
    }

    printf("Server is running at %s:%ld...\n", inet_ntoa(server_addres.sin_addr), args.port);
    int bytesrx;
    char buf[TFTP_DEFAULT_DATA_SIZE + 4];
    struct sockaddr_in client_address;
    socklen_t clientlen = sizeof(client_address);
    tftp_options_t options;

    while (1) {
        bytesrx = recvfrom(server_socket, buf, TFTP_DEFAULT_DATA_SIZE + 4, 0, (struct sockaddr *)&client_address, &clientlen);
        if (bytesrx < 0) {
            printf("Recvfrom error! \n");
            continue;
        }
        printf("some packet obtained \n");
        char mode_str[9];
        char file_path[4096]; // max unix path len
        strcpy(file_path, args.root_path);
        char two_buf[2];
        file_path[strlen(args.root_path)] = '/';
        int res = parse_req_packet(two_buf, file_path + 1 + strlen(args.root_path), mode_str, buf, bytesrx, &options);
        if (res == 1) {
            send_error(illegal_operation, "Wrong request format!", client_address, server_socket);
            continue;
        } else if (res == 2) {
            send_error(option_negotiation_error, "Option error - probably option value error!", client_address, server_socket);
            continue;
        }

        uint16_t code = short16_from_chars(two_buf);
        code = ntohs(code);
        int mode;
        if (strcmp(mode_str, "octet") == 0) {
            mode = octet_mode;
        } else if (strcmp(mode_str, "netascii") == 0) {
            mode = netascii_mode;
        } else {
            printf("Unknown mode! \n");
            send_error(not_defined, "Unknown mode!", client_address, server_socket);
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            printf("Fork error! \n");
            send_error(not_defined, "Creation of process(fork) failed!", client_address, server_socket);
        } else if (pid == 0) {
            if (code == read_req_opcode) {
                tftp_options_t options_for_print = options;
                print_info_rrq(client_address, mode, file_path + strlen(args.root_path), &options_for_print);

                tftp_read(client_address, mode, file_path, options);
            } else if (code == write_req_opcode) {
                tftp_options_t options_for_print = options;
                print_info_wrq(client_address, mode, file_path + strlen(args.root_path), &options_for_print);

                tftp_write(client_address, mode, file_path, options);
            } else {
                printf("Received packet has unknown opcode! \n");
                send_error(illegal_operation, "Wrong op code!", client_address, server_socket);
            }
        }
        // terminate any hanging zombie process but don't wait
        waitpid(-1, NULL, WNOHANG);
        // In case there's a lot of traffic. At least(later on) more is closed than created.
        waitpid(-1, NULL, WNOHANG);
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    signal(SIGINT, sig_handler);

    main_loop();

    free_and_exit(false);
    return 0;
}
