#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "codes.h"

uint16_t char_to_short(char buf[2]) {
    union {
        char ch[2];
        uint16_t n;
    } char2short;
    char2short.ch[0] = buf[0];
    char2short.ch[1] = buf[1];
    return char2short.n;
}

/*
    Just changes the goggles with  which compiler looks at the data
*/
inline char *short_to_char(uint16_t *short16) {
    return (char *)short16;
}

/*
    @return Returns int < 0 if an error occured
*/
int send_ack(uint16_t block_num, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons(ack);
    block_num = htons(block_num);
    char msg[ACK_PACKET_SIZE];
    sprintf(msg, "%hu%hu", op_code, block_num);

    return sendto(socket, msg, ACK_PACKET_SIZE, 0, (struct sockaddr *)&address, sizeof(address));
}

int send_error(uint16_t err_code, char *err_msg, struct sockaddr_in address, int socket) {
    uint16_t op_code = htons(error);
    err_code = htons(err_code);
}

int main() {
    uint16_t op_code = 258;
    uint16_t block_num = 1;

    char msg[ACK_PACKET_SIZE];
    int a = sprintf(msg, "%hu%hu", op_code, block_num);
    printf("%d: %c %c - %c %c \n", a, msg[0], msg[1], msg[2], msg[3]);
    printf("sizeof msg: %ld \n", sizeof(msg));
}
