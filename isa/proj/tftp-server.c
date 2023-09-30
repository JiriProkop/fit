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
#include <sys/wait.h>
#include <unistd.h>

#include "codes.h"

#define IP_PROTOCOL 0
#define MAX_PORT_NUM 65535
#define TFTP_DEFAULT_SERVER_PORT 69

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

void parse_args(int argc, char *argv[]) {
    if (argc == 2) { // TODO --help or -h
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
                printf("Invalid socket arg value! (must be integer > 0 - 0 is default)\n");
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

void read() {

    exit(EXIT_SUCCESS);
}

void write() {

    exit(EXIT_SUCCESS);
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

    printf("\nWaiting for stuff to happen...\n");
    int bytesrx;
    char buf[120];
    struct sockaddr_in client_address;
    socklen_t clientlen = sizeof(client_address);

    while (1) {
        // recvfrom blocks until msg is received or error encountered
        bytesrx = recvfrom(server_socket, buf, 120, 0, (struct sockaddr *)&client_address, &clientlen);
        if (bytesrx < 0) {
            printf("Recvfrom error! \n");
            free_and_exit(true);
        }
        printf("Msg: %s \n", buf);

        pid_t pid = fork();
        if (pid == -1) {
            printf("Fork error! \n");
            free_and_exit(true);
        } else if (pid == 0) {
            // child
            read();
            write();
        }
        // terminate any hanging zombie process but don't wait
        waitpid(-1, NULL, WNOHANG);
        // In case there's a lot of traffic. At least(later on) more is closed than created.
        waitpid(-1, NULL, WNOHANG);
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    printf("path: %s, port %lu \n", args.root_path, args.port);
    signal(SIGINT, sig_handler);

    main_loop();

    free_and_exit(false);
    return 0;
}
