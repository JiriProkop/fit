SERVER = tftp-server
CLIENT = tftp-client

CC = gcc
CFLAGS = -Wall -std=c11
LDLIBS =

SRCS = $(wildcard *.c)

.PHONY: clean all

all: $(SERVER)

$(SERVER):

clean:
	rm -f $(SERVER) $(CLIENT)

