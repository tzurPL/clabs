mcro math_ops
    add $1, $2, $3
    sub $4, $5, $6
    and $7, $8, $9
    or $10, $11, $12
    nor $13, $14, $15
mcroend

mcro copy_ops
    move $1, $2
    mvhi $3, $4
    mvlo $5, $6
mcroend

START: math_ops
       copy_ops
       addi $1, 100, $2
       subi $3, -50, $4
       andi $5, 255, $6
       ori $7, 128, $8
       nori $9, 0, $10
       stop

.db 10, -20, 30
.dh 1000, -2000, 3000
.dw 100000, -200000, 300000
.asciz "I Love the openuni"
