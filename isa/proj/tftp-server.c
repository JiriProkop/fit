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
#include <sys/wait.h>
#include <unistd.h>

#include "codes.h"

#define IP_PROTOCOL 0
#define MAX_PORT_NUM 65535
#define TFTP_DEFAULT_SERVER_PORT 69
#define TFTP_DEFAULT_DATA_SIZE 512 // FIXME this is true just for the data blocks, initial request can be longer

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
// BUG need to return proper fail codes, defined in codes.h
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

/*
    buf is expected to be already allocated to n size

    @return returns number of read chars
*/
size_t get_nchars_from_file(FILE *fp, size_t n, char *buf, int mode) {
    static int c = 0;
    size_t i = 0;
    // put left over letter from last reading
    if (mode == octet_mode) {
        if (c == '\n') {
            buf[i++] = c;
        } else if (c == '\r') {
            buf[i++] = '\0';
        }
    }
    for (; i < n; i++) {
        c = fgetc(fp);
        if (mode == octet_mode && (c == '\r' || c == '\n')) {
            buf[i] == '\r';
            if (++i == n) {
                return i;
            }
            if (c == '\r') {
                c = '\0';
            }
        }
        buf[i] = c;
        if (c == EOF) {
            return i;
        }
    }
    return i;
}

void tftp_read(struct sockaddr_in client_address, char msg[], int mode, const char file_name[]) {
    char buf[TFTP_DEFAULT_DATA_SIZE + 4]; // FIXME this is probably true only without extensions

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Couldn't read the file! (not present or insufficient privileges) \n");
        free_and_exit(true);
    }
    /* TODO always check source port num(has to be the same) An error packet
   should be sent to the source of the incorrect packet, while not
   disturbing the transfer.  This can be done only if the TFTP in fact
   receives a packet with an incorrect TID.  If the supportin"g protocols
   do not allow it, this particular error condition will not arise.     */

    /*
        2 bytes     string    1 byte     string   1 byte
        ------------------------------------------------
       | Opcode |  Filename  |   0  |    Mode    |   0  |
        ------------------------------------------------
    */
    while (1) {
        size_t data_size = get_nchars_from_file(fp, TFTP_DEFAULT_DATA_SIZE, buf, mode);
        // send packet, if data_size < TFTP_DEFAULT_DATA_SIZE, break
    }

    exit(EXIT_SUCCESS);
}

void tftp_write(struct sockaddr_in client_address, char msg[], int mode, const char file_name[]) {
    // if file access is ok: send ACK with block num 0
    // TODO always check source port num(has to be the same)

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

    printf("Waiting for stuff to happen...\n");
    int bytesrx;
    // FIXME 1st packet can be bigger than this, so you should check the size
    char buf[TFTP_DEFAULT_DATA_SIZE];
    struct sockaddr_in client_address;
    socklen_t clientlen = sizeof(client_address);

    while (1) {
        // recvfrom blocks until msg is received or error encountered
        bytesrx = recvfrom(server_socket, buf, TFTP_DEFAULT_DATA_SIZE, 0, (struct sockaddr *)&client_address, &clientlen);
        if (bytesrx < 0) {
            printf("Recvfrom error! \n");
            continue;
        }
        char mode_str[9];
        char file_name[256];
        char two_buf[2];
        if(sscanf(buf, "%c%c%s%s", two_buf[0], two_buf[1], file_name, mode_str) == EOF){
            printf("Wrong request format in msg! \n");
            continue;
        }
        for (int i = 0; mode_str[i]; ++i)
            mode_str[i] = tolower(mode_str[i]);

        uint16_t code = opcode_from_chars(two_buf);
        code = ntohs(code);
        int mode;
        if (strcmp(mode_str, "octet") == 0) {
            mode = octet_mode;
        } else if(strcmp(mode_str, "netascii") == 0){
            mode = netascii_mode;
        } else{
            printf("Unknown mode! \n");
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            printf("Fork error! \n");
            free_and_exit(true);
        } else if (pid == 0) { // child
            if (code == read_req_opcode) {
                tftp_read(client_address, buf, mode, file_name);
            } else if (code == write_req_opcode) {
                tftp_write(client_address, buf, mode, file_name);
            } else {
                printf("Received packet has unknown opcode! \n");
                exit(EXIT_FAILURE);
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
