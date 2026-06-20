mcro asciz
    add $5, $5, $5
    addi $5, 10, $5
mcroend

.extern L_EXT
.entry MAIN
MAIN:   la L_EXT
        asciz
        jmp L_EXT
        hlt
