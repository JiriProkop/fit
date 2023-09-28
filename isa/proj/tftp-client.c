#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
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

#include "codes.h"

#define IP_PROTOCOL 0
#define MAX_PORT_NUM 65535

typedef struct {
    char *hostname;
    char *file_path;
    char *dest_file_path;
    long port; // FIXME sjednotit nazev se serverem
} args_t;

args_t args = {.hostname = NULL, .file_path = NULL, .dest_file_path = NULL, .port = 0};

int client_socket;
bool close_socket = false;

/*
    Free allocated resources and exit.

    @param failure true for EXIT_FAILURE return code, EXIT_SUCCESS otherwise
*/
void free_and_exit(bool failure) {
    free(args.hostname);
    free(args.file_path);
    free(args.dest_file_path);

    if (close_socket) {
        close(client_socket);
    }

    if (failure) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}

void sig_handler(int _) {
    (void)_;
    printf("\nSIGINT received, clearing allocations.\n");
    free_and_exit(false);
}

void parse_args(int argc, char *argv[]) {
    // if argc == 2 && argv[1] == "--help": show help

    int opt;
    while ((opt = getopt(argc, argv, "h:p:f:t:")) != -1) {
        switch (opt) {
            case 'h':
                args.hostname = malloc(strlen(optarg + 1));
                if (args.hostname == NULL) {
                    printf("Memory allocation error! \n");
                    free_and_exit(true);
                }
                strcpy(args.hostname, optarg);
                break;
            case 'p':
                char *check;
                args.port = strtoul(optarg, &check, 10);
                if (args.port < 0 || args.port > MAX_PORT_NUM) {
                    printf("Invalid socket number!(must be between 0 and %d)\n", MAX_PORT_NUM + 1);
                    exit(EXIT_FAILURE);
                } else if (args.port == 0) {
                    printf("Invalid socket arg value! (must be integer > 0)\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                args.file_path = malloc(strlen(optarg + 1));
                if (args.file_path == NULL) {
                    printf("Memory allocation error! \n");
                    free_and_exit(true);
                }
                strcpy(args.file_path, optarg);
                break;
            case 't':
                args.dest_file_path = malloc(strlen(optarg + 1));
                if (args.dest_file_path == NULL) {
                    printf("Memory allocation error! \n");
                    free_and_exit(true);
                }
                strcpy(args.dest_file_path, optarg);
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
                // TODO print help
                free_and_exit(true);
        }
    }
    if (args.hostname == NULL || args.dest_file_path == NULL) {
        printf("%s, %s \n", args.hostname, args.dest_file_path);
        printf("-h and -t arguments are required! \n");
        // TODO show help
        free_and_exit(true);
    }
}

void logic() {
    // int nBytes;
    struct sockaddr_in server_address;
    int addrlen = sizeof(server_address);
    server_address.sin_addr.s_addr = INADDR_ANY;
    struct hostent *server;
    // FILE *fp;

    /*
        The next few lines of code were taken from: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp/client.c
        Author: Ondrej Rysavy (rysavy@fit.vutbr.cz)
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
    // end of taken code

    client_socket = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
    if (client_socket < 0) {
        printf("Socket creation error!\n");
        free_and_exit(true);
    }

    /* odeslani zpravy na server */
    char buf[] = "Hello server!";
    int bytestx = sendto(client_socket, buf, strlen(buf), 0, (struct sockaddr *)&server_address, addrlen);
    if (bytestx < 0) {
        printf("Sendto error! \n");
        free_and_exit(true);
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    signal(SIGINT, sig_handler);

    logic();

    return 0;
}
