mcro load_pair
    lb $1, 0, $2
    lh $3, 2, $4
mcroend

mcro store_pair
    sb $1, 0, $2
    sh $3, 2, $4
mcroend

.entry INIT

INIT: addi $0, 5, $1
      addi $0, 10, $2
      jmp $31
      load_pair
      store_pair
      lw $5, 100, $6
      sw $7, -100, $8
      move $1, $3
      mvhi $2, $4
      mvlo $5, $6
      stop

TABLE: .db 0, 127, -128
       .dh 32767, -32768
       .dw 2147483647, -1
       .asciz "test4"
