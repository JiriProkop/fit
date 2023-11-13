SERVER = tftp-server
CLIENT = tftp-client

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDLIBS =
DEPS = tftp.c tftp.h


.PHONY: clean all

all: $(SERVER) $(CLIENT)

$(SERVER): $(DEPS)

$(CLIENT): $(DEPS)


clean:
	rm -f $(SERVER) $(CLIENT)

