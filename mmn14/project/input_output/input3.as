; input3
.extern EXTERN_FUNC
.extern EXTERN_DATA
.entry ENTRY_POINT

mcro setup
    la EXTERN_DATA
    lw $5, 0, $6
mcroend

ENTRY_POINT: setup
    addi $1, 10, $2

LOOP: call EXTERN_FUNC
    subi $2, 1, $2
    bgt $2, $0, LOOP

    stop
