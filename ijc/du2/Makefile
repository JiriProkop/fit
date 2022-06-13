# Makefile
# Řešení IJC-DU2, 18.4.2022
# Autor: Jiří Prokop, FIT
# Přeloženo: gcc 9.4.0

LC_ALL=cs._CZ.utf8
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g
CC = gcc

all:
	make tail

tail:
	$(CC) $(CFLAGS) tail.c -o $@

clean:
	rm tail

zip:
	zip xproko47.zip *.c *.cc *.h Makefile