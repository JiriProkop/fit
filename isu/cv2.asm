; (1) Vytvorte zakladnu strukturu programu (sekcie + include kniznice rw32) + nezabudnite na instrukciu ret na konci
;     - Na zaciatku funkcie main nezabudnite na:    push ebp
;                                                   mov ebp, esp
;     - Na konci funkcie nezabudnite na:    pop ebp
;                                           ret
;
; (2) V spravnych sekciach zadefinujte nasludujuce premenne
;     - "varA" s hodnotou -0x0842 a "varB" s hodnotou 65536 (urcite najmensi mozny datovy typ)
;     - "login", ktora obsahuje retazec s vasim loginom, zakonceny 0 znakom
;     - neinicializovana premenna "varC" o velkosti 16b
;
; (3) V spravnej sekcii zadefinujte nasledovne premenne (cislo v nazve premennej znaci pocet bitov)
;
;     x8  ?? 12
;     u8  ?? 132
;     i8  ?? -6
;
;     x16 ?? 10658
;     u16 ?? 63000
;     i16 ?? -318
;
;     x32 ?? 2
;     u32 ?? 3100000000
;     i32 ?? -1
;
;     x64 ?? 10000000000
;     i64 ?? -1024
;
; (4) Vypocitajte zadane vyrazi
;     - Vysledok vypiste pomocou spravnej funkcie z kniznice rw32 (Write(U)IntXX)
;     - Pre 64b cisla su pripravene funkcie na vypis na konci tohoto suboru
;     - Po kazdom vypise vypiste pre prehladnost novy riadok: call WriteNewLine
;     - Instrukcie: INC, DEC, NEG, ADD, ADC, SUB, SBB, MUL, IMUL, DIV, IDIV, CBW, CWD, CDQ
;
;     x8 + 1
;     i8 % x8 
;     x16 - i16
;     x16 * u16
;     x32 * 8
;     x64 + i64     V tomto pripade, musite scita po 32b castiach
;     u32 / i32 
;     u32 / u16 
;     u32 / i8 
;     x64 / u32 
;     x64 / x32 
;     -x32 
;
; (5) Vypocitajte vyraz (login[0:3] ^ login[4:7])
;     - Pomocou debuggeru skontrolujte, ci je nastaveny flag PF
;     - Pokial ano, spocitajte vyraz: 
;           (varA * 42 + varB) / varA
;     - Pokial nie, spocitajte vyraz:
;           (varA * 42 + varB) / varB
;     - Nezabudnite jednotlive vyrazi znamienkovo rozsirit pokial treba
;     - Vysledok vypiste pomocou funkcie z kniznice rw32 (WriteIntXX, WriteNewLine)
;
; ========================== Sem piste vase riesenie ==========================

t






; =============================================================================


; Funkcie na vypis 64b cisel
; ocakavaju hodnoty v edx:eax

CEXTERN printf
section .data
pf_fmt_i db "%lld",0
pf_fmt_u db "%llu",0

section .text
WriteInt64:
    push ebp
    mov ebp,esp

    push edx
    push eax
    push pf_fmt_i
    call printf
    add esp, 12

    leave
    ret

WriteUInt64:
    push ebp
    mov ebp,esp

    push edx
    push eax
    push pf_fmt_u
    call printf
    add esp, 12

    leave
    ret