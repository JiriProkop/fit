EXEC = square
CC = gcc
CFLAGS = -Wall -std=c11 -MMD
LDLIBS = -lm

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
DEPS = $(SRCS:.c=.d)

.PHONY: clean all

all: $(EXEC)

$(EXEC): $(OBJS)

-include $(DEPS)

clean:
	rm -f $(EXEC) $(OBJS) $(DEPS)

