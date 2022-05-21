// error.h
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#ifndef error
#define error

void warning_msg(const char *fmt, ...);

void error_exit(const char *fmt, ...);

//  Tyto funkce mají
//  stejné parametry jako printf(); tisknou text "CHYBA: " a potom
//  chybové hlášení podle formátu fmt. Vše se tiskne do stderr
//  (funkcí vfprintf) a potom pouze error_exit ukončí program voláním
//  funkce exit(1)


#endif