/*
    Autor: Jiří Prokop, xproko47
    File: tftp-client.c
*/

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "tftp.h"

// Struct for program arguments.
typedef struct {
    char *hostname;
    char *to_transfer_file_path; // path to file on server(download) or nothing(upload, stdin is used)
    char *future_file_path;      // path where the file will be saved
    long port;
} args_t;

args_t args = {.hostname = NULL, .to_transfer_file_path = NULL, .future_file_path = NULL, .port = TFTP_DEFAULT_SERVER_PORT};

int client_socket;
bool close_socket = false;
u_int16_t mode = octet_mode;

/*
    Free allocated resources and exit.

    @param failure true for EXIT_FAILURE return code, EXIT_SUCCESS otherwise
*/
void free_and_exit(bool failure) {
    free(args.hostname);
    free(args.to_transfer_file_path);
    free(args.future_file_path);

    if (close_socket) {
        close(client_socket);
    }

    if (failure) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}

/*
    Signal handler for SIGINT
*/
void sig_handler(int _) {
    (void)_;
    printf("\nSIGINT received, clearing allocations.\n");
    free_and_exit(false);
}

/*
    Prints help.
*/
void print_help() {
    printf("Usage: tftp-client -h hostname [-p port] [-f filepath] -t dest_filepath\n");
    printf("  -h   IP address/domain name of the remote server\n");
    printf("  -p   Port of the remote server (default is specified in the RFC TFTP(69))\n");
    printf("  -f   Path to the file to be downloaded from the server (download)\n");
    printf("       If not specified, content from stdin will be used (upload)\n");
    printf("  -t   Path under which the file will be stored on the remote server or locally\n");
}

/*
    Parses program arguments.

    @param argc number of arguments
    @param argv array of arguments
*/
void parse_args(int argc, char *argv[]) {
    if (argc == 2 && (!strcmp(argv[1], "--help"))) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    char *check;
    int opt;
    while ((opt = getopt(argc, argv, "h:p:f:t:")) != -1) {
        switch (opt) {
            case 'h':
                args.hostname = malloc(strlen(optarg) + 1);
                if (args.hostname == NULL) {
                    printf("Memory allocation error! \n");
                    free_and_exit(true);
                }
                strcpy(args.hostname, optarg);
                break;
            case 'p':
                args.port = strtoul(optarg, &check, 10);
                if (args.port < 0 || args.port > MAX_PORT_NUM) {
                    printf("Invalid socket number!(must be between 0 and %d)\n", MAX_PORT_NUM + 1);
                    free_and_exit(true);
                } else if (args.port == 0) {
                    printf("Invalid socket arg value! (must be integer > 0)\n");
                    free_and_exit(true);
                }
                break;
            case 'f':
                args.to_transfer_file_path = malloc(strlen(optarg) + 1);
                if (args.to_transfer_file_path == NULL) {
                    printf("Memory allocation error! \n");
                    free_and_exit(true);
                }
                strcpy(args.to_transfer_file_path, optarg);
                break;
            case 't':
                args.future_file_path = malloc(strlen(optarg) + 1);
                if (args.future_file_path == NULL) {
                    printf("Memory allocation error! \n");
                    free_and_exit(true);
                }
                strcpy(args.future_file_path, optarg);
                break;
            case '?':
                if (optopt == 'h' || optopt == 'p' || optopt == 'f' || optopt == 't')
                    printf("Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    printf("Unknown option `-%c'.\n", optopt);
                else
                    printf("Unknown option character `\\x%x'.\n", optopt);
                free_and_exit(true);
                break;
            default:
                free_and_exit(true);
        }
    }
    if (args.hostname == NULL || args.future_file_path == NULL) {
        printf("-h and -t arguments are required! \n");
        print_help();
        free_and_exit(true);
    } else if (args.future_file_path != NULL && args.to_transfer_file_path == NULL && access(args.future_file_path, F_OK) == 0) {
        printf("File '%s' already exists \n", args.future_file_path);
        free_and_exit(true);
    }
}

/*
    Function for read request communication.

    @param server_address server address
    @param socket socket
*/
void read_req(struct sockaddr_in server_address, int socket) {
    char buf[TFTP_DEFAULT_DATA_SIZE + 4]; // without extensions
    FILE *fp = fopen(args.future_file_path, "w");
    if (fp == NULL) {
        printf("Couldn't create the file! (probably insufficient privileges) %d \n", errno);
        free_and_exit(true);
    }
    int bytestx;
    socklen_t addr_len = sizeof(server_address);
    bool to_break = false;
    int prev_client_port;
    struct in_addr prev_ip;
    uint16_t block_num = 1;
    bool first_packet = true;

    while (1) {
    client_sending_packet_read:
        for (int i = 0; i < RETRY_SENT_COUNT; i++) {
            if (!set_socket_exp_timeout(socket, i)) { // exponential timeout increase
                send_error(not_defined, "Error setting timeout!", server_address, socket);
                remove(args.future_file_path);
                fclose(fp);
                free_and_exit(true);
            }
            if (first_packet) {
                if (send_request(read_req_opcode, args.to_transfer_file_path, "octet", server_address, socket) < 0) {
                    printf("Sendto error! \n");
                    continue;
                }
            } else {
                if (send_ack(block_num - 1, server_address, socket) < 0) {
                    printf("Sendto error! \n");
                    continue;
                }
                if (to_break) {
                    goto client_end_loop_read;
                }
            }
            // wait for data
            bytestx = recvfrom(socket, buf, TFTP_DEFAULT_DATA_SIZE + 4, 0, (struct sockaddr *)&server_address, &addr_len);
            if (bytestx < 0) {
                printf("Recvfrom error or timeout! \n");
                continue;
            }
            if (!first_packet && (server_address.sin_addr.s_addr != prev_ip.s_addr || server_address.sin_port != prev_client_port)) {
                send_error(unknown_tid, "Unknown sender address or TID!", server_address, socket);
                server_address.sin_addr = prev_ip;
                server_address.sin_port = prev_client_port;
                continue;
            }
            size_t data_size = bytestx - 4;
            uint16_t err_code;
            uint16_t packet_block_num;
            uint16_t code = short16_from_chars(buf);
            code = ntohs(code);
            switch (code) {
                case error_opcode:
                    err_code = short16_from_chars(buf + 2);
                    err_code = ntohs(code);
                    print_info_err(server_address, err_code, buf + 4, get_port_from_socket(socket));
                    remove(args.future_file_path);
                    fclose(fp);
                    free_and_exit(true);
                    break;
                case data_opcode:
                    packet_block_num = ntohs(short16_from_chars(buf + 2));
                    print_info_data(server_address, packet_block_num, get_port_from_socket(socket));

                    if (block_num != packet_block_num) {
                        continue;
                    }
                    if (first_packet) {
                        first_packet = false;
                        prev_ip = server_address.sin_addr;
                        prev_client_port = server_address.sin_port;
                    }
                    to_break = data_size < TFTP_DEFAULT_DATA_SIZE;

                    printf("writing file... \n");
                    text_from_mode(octet_mode, buf + 4, data_size); // convert from netascii to normalascii
                    // write
                    for (size_t j = 0; j < data_size; j++) {
                        fputc(buf[4 + j], fp);
                    }
                    block_num++;
                    goto client_sending_packet_read;
                case oack_opcode:
                case ack_opcode:
                case read_req_opcode:
                case write_req_opcode:
                    print_info_not_parsed(server_address, buf, bytestx, socket);
                    break;
                default:
                    printf("Received packet has unknown opcode! \n");
                    send_error(illegal_operation, "Wrong request format(unknown opcode)!", server_address, socket);
                    remove(args.future_file_path);
                    fclose(fp);
                    free_and_exit(true);
            }
        }
        remove(args.future_file_path);
        send_error(not_defined, "Timed out!", server_address, socket);
        free_and_exit(true);
    }
client_end_loop_read:
    fclose(fp);
    free_and_exit(false);
}

/*
    Function for write request communication.

    @param server_address server address
    @param socket socket
*/
void write_req(struct sockaddr_in server_address, int socket) {
    char buf[TFTP_DEFAULT_DATA_SIZE + 4];

    FILE *fp = stdin;

    int bytestx;
    socklen_t addr_len = sizeof(server_address);
    bool to_break = false;
    int prev_client_port;
    struct in_addr prev_ip;
    uint16_t block_num = 0;
    size_t data_size;
    bool first_packet = true;

    while (1) {
    client_sending_packet_write:
        if (!first_packet) {
            printf("getting data \n");
            data_size = get_nchars_from_file(fp, TFTP_DEFAULT_DATA_SIZE, buf, mode);
            printf("sending data of size: %ld \n", data_size);
            to_break = data_size < TFTP_DEFAULT_DATA_SIZE;
        }
        for (int i = 0; i < RETRY_SENT_COUNT; i++) {
            if (!set_socket_exp_timeout(socket, i)) {
                send_error(not_defined, "Error setting timeout!", server_address, socket);
                fclose(fp);
                free_and_exit(true);
            }
            if (first_packet) {
                if (send_request(write_req_opcode, args.future_file_path, "octet", server_address, socket) < 0) {
                    printf("Sendto error(write req)! \n");
                    continue;
                }
            } else {
                bytestx = send_data(block_num, buf, data_size, server_address, socket);
                if (bytestx < 0) {
                    printf("Sendto error(send data)! \n");
                    continue;
                }
            }
            // wait for ack
            bytestx = recvfrom(socket, buf, TFTP_DEFAULT_DATA_SIZE + 4, 0, (struct sockaddr *)&server_address, &addr_len);
            if (bytestx < 0) {
                printf("Recvfrom error or timeout! \n");
                continue;
            }
            if (!first_packet && (server_address.sin_addr.s_addr != prev_ip.s_addr || server_address.sin_port != prev_client_port)) {
                send_error(unknown_tid, "Unknown sender address or TID!", server_address, socket);
                server_address.sin_addr = prev_ip;
                server_address.sin_port = prev_client_port;
                continue;
            }
            // check ack, print info
            uint16_t packet_block_num;
            uint16_t err_code;
            uint16_t code = short16_from_chars(buf);
            code = ntohs(code);
            switch (code) {
                case error_opcode:
                    err_code = short16_from_chars(buf + 2);
                    err_code = ntohs(code);
                    print_info_err(server_address, err_code, buf + 4, get_port_from_socket(socket));

                    free_and_exit(true);
                    break;
                case ack_opcode:
                    packet_block_num = ntohs(short16_from_chars(buf + 2));
                    print_info_ack(server_address, packet_block_num);

                    if (block_num != packet_block_num) {
                        continue;
                    } else {
                        if (first_packet) {
                            first_packet = false;
                            prev_ip = server_address.sin_addr;
                            prev_client_port = server_address.sin_port;
                        }
                        if (to_break) {
                            goto client_end_loop_write; // this was the last packet, we can leave
                        }
                        block_num++;
                        goto client_sending_packet_write;
                    }
                    break;
                case oack_opcode:
                case data_opcode:
                case read_req_opcode:
                case write_req_opcode:
                    print_info_not_parsed(server_address, buf, bytestx, socket);
                    break;
                default:
                    printf("Received packet has unknown opcode! \n");
                    send_error(illegal_operation, "Wrong request format(unknown opcode)!", server_address, socket);
                    free_and_exit(true);
            }
        }
        // didnt get ack for the RETRY_SENT_COUNTth time -> quiting
        send_error(not_defined, "Timed out!", server_address, socket);
        free_and_exit(true);
    }
client_end_loop_write:
    free_and_exit(false);
}

/*
    Setup for communication. Calls read_req or write_req at the end.
*/
void logic() {
    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr = INADDR_ANY;
    struct hostent *server;
    /*
        The next few lines of code were taken from:
       https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp/client.c Author: Ondrej Rysavy (rysavy@fit.vutbr.cz)
    */
    if ((server = gethostbyname(args.hostname)) == NULL) {
        printf("ERROR: no such host as %s\n", args.hostname);
        free_and_exit(true);
    }
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(args.port);
    printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
    /*
        end of taken code
    */

    client_socket = create_socket_for_process(0);
    if (client_socket < 0) {
        printf("Socket creation error!\n");
        free_and_exit(true);
    }
    close_socket = true;

    if (args.to_transfer_file_path == NULL) {
        write_req(server_address, client_socket);
    } else {
        read_req(server_address, client_socket);
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    signal(SIGINT, sig_handler);

    logic();

    return 0;
}
