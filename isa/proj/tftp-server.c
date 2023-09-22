#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    unsigned long port_num; // 0 to 65536
    char *root_path;
} args_t;

// tftp-server [-p port] root_dirpath
args_t parse_args(int argc, char *argv[]) {
    args_t args = {.root_path = NULL, .port_num = 0};
    if (argc == 2) { // TODO --help or -h
        args.root_path = malloc(strlen(argv[1] + 1));
        strcpy(args.root_path, argv[1]);
    } else if (argc == 4) {
        if (!strcmp(argv[1], "-p")) {
            char *check;
            args.port_num = strtoul(argv[1], &check, 10);

            args.root_path = malloc(strlen(argv[3] + 1));
            strcpy(args.root_path, argv[3]);
        } else {
            printf("Wrong argument name '-p' or it's position!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Wrong argument count!\n");
        exit(EXIT_FAILURE);
    }
    return args;
}

int main(int argc, char *argv[]) {
    args_t args = parse_args(argc, argv);
    printf("path: %s, port %lu \n", args.root_path, args.port_num);
    free(args.root_path);
    return 0;
}
