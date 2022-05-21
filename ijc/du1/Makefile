# Makefile
# Řešení IJC-DU1, 20.3.2022
# Autor: Jiří Prokop, FIT
# Přeloženo: gcc 9.4.0

LC_ALL=cs._CZ.utf8
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g
CC = gcc
LDLIBS = -lm

all:
	make primes
	make primes-i
	make steg-decode
	
primes: primes.c eratosthenes.c eratosthenes.h bitset.c bitset.h error.c error.h
	$(CC) $(CFLAGS) -O2 $^ -o $@ $(LDLIBS)
	
primes-i: primes.c eratosthenes.c eratosthenes.h bitset.c bitset.h error.c error.h
	$(CC) -DUSE_INLINE $(CFLAGS) -O2 $^ -o $@ $(LDLIBS)
	
run:
	make primes
	make primes-i
	./primes
	./primes-i

steg-decode: steg-decode.c ppm.c ppm.h error.c error.h eratosthenes.c eratosthenes.h bitset.c bitset.h
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm primes-i primes steg-decode

zip:
	zip xproko47.zip *.c *.h Makefile
