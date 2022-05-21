// error.c
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

 void warning_msg(const char *fmt, ...)
 {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
 }

void error_exit(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    va_end(args);

    exit(1);
}