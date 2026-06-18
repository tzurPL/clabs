; file: edge_sneaky_err.as
START:  add $1, $2, $3 garbage     ; Err: Extra text
        .db 10, 20,                ; Err: Trailing comma
        .db , 10, 20               ; Err: Leading comma
        .asciz "Missing end quote  ; Err: Missing quote
        jmp $0, $1                 ; Err: jmp takes only 1 operand
        hlt
