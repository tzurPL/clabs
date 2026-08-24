.extern EXT1
.extern EXT2
.entry A
.entry B

A: addi $0, 0, $1
   beq $1, $0, B
   call EXT1
   la EXT2
   jmp A

B: add $1, $2, $3
   sub $3, $2, $1
   and $4, $5, $6
   or $7, $8, $9
   nor $10, $11, $12
   bne $1, $3, A
   blt $3, $1, B
   bgt $0, $0, B
   stop

STR: .asciz "AB"
NUMS: .db 1
      .dh 2
      .dw 3
