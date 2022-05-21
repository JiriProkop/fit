EXEC = proj2
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
SRCS = $(wildcard *.c)
LDLIBS = -lpthread -lrt

.PHONY: clean

$(EXEC):

clean:
	rm -f $(EXEC)
	rm -f proj2.out

zip:
	zip $(EXEC) Makefile $(SRCS)