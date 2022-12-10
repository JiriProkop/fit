; Autor reseni: Jiří Prokop xproko47

; Projekt 2 - INP 2022
; Vernamova sifra na architekture MIPS64

; DATA SEGMENT
                .data
login:          .asciiz "xproko47"  ; sem doplnte vas login
cipher:         .space  17  ; misto pro zapis sifrovaneho loginu

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize "funkce" print_string)


; CODE SEGMENT
                .text

                ; ZDE NAHRADTE KOD VASIM RESENIM
main:
				daddi	r28, r0, 16		; p
				daddi	r26, r0, 18		; r
loop:
				; 1st char ------------------------------
				lbu 	r11, login(r4)
				;check if r11 is not a number
				daddi	r16, r0, 97
				sltu	r16, r11, r16
				bne		r16, r0, end ; if < 97('a') end

				daddi	r16, r0, 97
				sub 	r11, r11, r16 	; -'a'
				add		r11, r11, r28	; + 16
				daddi	r16, r0, 26
				div		r11, r16
				mfhi	r11
				daddi	r11, r11, 97	; +'a'
				sb		r11, cipher(r4)
				; 2nd char ------------------------------
				daddi	r4, r4, 1
				lbu 	r11, login(r4)
				;check if r11 is not a number
				daddi	r16, r0, 97
				sltu	r16, r11, r16
				bne		r16, r0, end ; if < 97('a') end

				daddi	r16, r0, 97
				sub 	r11, r11, r16 	; -'a'
				sub		r11, r11, r26

				slti 	r16, r11, 0
				bne		r16, r0, negative
				daddi	r16, r0, 26
				div		r11, r16
				mfhi	r11
				daddi	r11, r11, 97	; +'a'
				b 		done_2
negative:
				daddi	r11, r11, 122
				daddi	r11, r11, 1
done_2:
				sb		r11, cipher(r4)
				daddi	r4, r4, 1
				b 		loop
end:
				daddi	r4, r4, 1
				daddi	r11, r0, 0
				sb		r11, cipher(r4)
				
				xor 	r0, r0, r0
				daddi   r4, r0, cipher   ; vzorovy vypis: adresa login: do r4
                jal     print_string    ; vypis pomoci print_string - viz nize


                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
