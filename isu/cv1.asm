%include "rw32-2021.inc"

section .data

    ; Uloha 0
    ; Definujte najmensi mozny datovy typ
    number db -128


    chars db "0123456789abcdefghijklmnopqrstuvwxyz",0 ; 36 znakov
    variables dw 42H, 1234H, 4567H, 7412H, 4561H, 5566H, 1245H, 9999H
    string db "wooo", 0

    endianB db 0x12, 0x34, 0x56, 0x78
    endianD dd 0x12345678

    structPerson    db "Jan Novak", 0
                    dd 764387234
                    dw 42

section .bss
    login resb 9
    vysledek resw 1
    a resb 1
    b resd 1
    c resw 1
    d resw 1
    e resw 1

section .text
CMAIN:
    push ebp
    mov ebp, esp

    ; Uloha 1
    ; Nacitajte hodnotu z adresy "number" a za jej pouzitia:
	; Nastavte priznaky CF=1, OF=1, PF=0
	; Povolene instrukcie: MOV, XOR, ADD

    mov al, [number]
    add al, [number]

    call WriteFlags
    call WriteInt8
    call WriteNewLine

    ; Uloha 2
    ; V sekcii .bss rezervujte velkost pola 9 bytov (resx)
    ; Do tohoto pola zapiste pomocou registra ESI svoj login (xloginNN)
    ; Velikost literalu musite specifikovat pomocou klucoveho slova (byte, word, dword, qword)

    mov ESI, login
    mov [ESI], byte 'x'
    mov [ESI + 1], byte 'p'
    mov [ESI + 2], byte 'r'
    mov [ESI + 3], byte 'o'
    mov [ESI + 4], byte 'k'
    mov [ESI + 5], byte 'o'
    mov [ESI + 6], byte '4'
    mov [ESI + 7], byte '7'


    call WriteString
    call WriteNewLine

    ; Uloha 3
    ; Pomocou pola "chars" vypiste svoj login
    ; Indexujte pomocou registra ESI + offset

    mov esi, chars


    call WriteString
    call WriteNewLine

    ; ; Uloha 4
    ; ; Pomocou bazoveho registru EBX vyberte z pola "variables" 2. a 7. prvok (indexujeme od nuly)
    ; ; Scitajte tieto prvky pomocou instrukcie ADD
    ; ; Reservujte v sekcii .bss premennu, do ktorej ulozite vysledok operacie
    ; ; Vysledok vypiste ako hexadecimalnu hodnotu (WriteHexXX)
    mov ebx, [variables + 4]
    add ebx, [variables + 14]
    mov [vysledek], eax


    
    ; call WriteNewLine

    ; ; Uloha 5
    ; ; Pomocou bazoveho registru EBX vyberte z pola variables 6. a 3. prvok (indexujeme od nuly)
    ; ; Odcitajte tieto prvky pomocou instrukcie SUB
    ; ; Reservujte v sekcii .bss premennu, do ktorej ulozite vysledok operacie
    ; ; Vysledok vypiste ako hexadecimalnu hodnotu (WriteHexXX)



    ; call WriteNewLine


    ; ; Uloha 6
    ; ; Pomocou bazoveho registra EBX vypiste premenne z adries "endianB" a "endianD"
    ; ; Hodnoty vypisujte hexadecimalne po jednotlivych bytoch (WriteHex8)
    task12:
        mov eax, [c]
        CWD
        imul dword [b]  ;eax, edx

        movsx ecx, byte [a]
        sub ecx, dword 21

        mov esi, eax
        mov ecx, edx

        add esi, ecx
        adc ecx, dword 0    ;ecx:esi

        mov al, 6
        CBW
        imul word [d]   ;dx:ax
        movsx eax, dx
        rol eax, 16
        movsx edx, ax
        add eax, edx
        mov edx, eax    ;edx

        mov ax, [e]
        sub ax, 121
        cwd
        add eax, edx

        mov edx, ecx
        mov ecx, eax
        mov esi, eax

        IDIV ecx

        mov [q], eax
        mov [r], edx

    ret

    
    ; call WriteNewLine

    ; ; Uloha 7
    ; ; Pomocou bazoveho registra EBX vypiste premenne v strukture "structPerson"
    ; ; (WriteIntXX, WriteString)


    ; call WriteNewLine

    ; ; Uloha 8
    ; ; Upravte retazec "string" tak, aby obsahoval 4 po sebe iduce znaky "wooo" -> "wxyz"
    ; ; Povolene instrukcie: MOV, INC 
    mov al, 'x'
    mov [string + 1], al
    mov al, 'y'
    mov [string + 2], al
    mov al, 'z'
    mov [string + 3], al

    ret

