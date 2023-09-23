#include <dirent.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "codes.h"

#define IP_PROTOCOL 0

typedef struct {
    long port_num; // 0 to 65536
    char *root_path;
} args_t;

args_t args = {.root_path = NULL, .port_num = 0};
int client_socket;
bool close_socket = false;

/*
    Free allocated resources and exit.

    @param failure true for EXIT_FAILURE return code, EXIT_SUCCESS otherwise
*/
void free_and_exit(bool failure) {
    free(args.root_path);
    if (close_socket) {
        close(client_socket);
    }

    if (failure) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}

// tftp-server [-p port] root_dirpath
void parse_args(int argc, char *argv[]) {
    if (argc == 2) { // TODO --help or -h
        args.root_path = malloc(strlen(argv[1] + 1));
        if (args.root_path == NULL) {
            printf("Memory allocation error!\n");
            exit(EXIT_FAILURE);
        }
        strcpy(args.root_path, argv[1]);
    } else if (argc == 4) {
        if (!strcmp(argv[1], "-p")) {
            char *check;
            args.port_num = strtoul(argv[1], &check, 10);
            if (args.port_num < 0) {
                printf("Invalid socket number! (must be > 0)\n");
                exit(EXIT_FAILURE);
            }

            args.root_path = malloc(strlen(argv[3] + 1));
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
        exit(EXIT_FAILURE);
    }
    // check if dir exists
    DIR *dir = opendir(args.root_path);
    if (dir) {
        closedir(dir); // Directory exists.
    } else if (ENOENT == errno) {
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

void logic() {
    // int nBytes;
    struct sockaddr_in addr_con;
    // int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(args.port_num);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    // FILE *fp;

    // socket()
    client_socket = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

    if (client_socket < 0) {
        printf("Socket creation error!\n");
        free_and_exit(true);
    }

    if (bind(client_socket, (struct sockaddr *)&addr_con, sizeof(addr_con)) < 0) {
        printf("Socket binding failed!\n");
        free_and_exit(true);

    } else {
        close_socket = true;
    }

    printf("\nWaiting for stuff to happen...\n");
    while (1) {
        ;
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    printf("path: %s, port %lu \n", args.root_path, args.port_num);
    signal(SIGINT, sig_handler);

    logic();
    // end
    free_and_exit(false);
    return 0;
}
