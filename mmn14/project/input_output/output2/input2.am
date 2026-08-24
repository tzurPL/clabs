.entry MAIN

MAIN: la DATA_START
      lw $1, 0, $2
      lh $3, 2, $4
      lb $5, 4, $6

LOOP: beq $1, $3, END_LOOP
      bne $1, $3, NEXT
NEXT: blt $1, $3, LESS
      bgt $1, $3, GREATER

LESS: sb $5, 0, $6
      jmp LOOP

GREATER: sh $3, 0, $4
      sw $1, 0, $2
      jmp LOOP

END_LOOP: stop

DATA_START: .asciz "Data"
