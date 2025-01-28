MAIN: add r3, LIST
LOOP: prn #48
macr macr3
cmp r3, #-6
bne END
endmacr
lea STR, r6
inc r6
mov *r6,K
sub r1, r4
macr3
dec K
jmp LOOP
END: stop
STR: .string "abcd"
LIST: .data 6, -9
K: .data -100