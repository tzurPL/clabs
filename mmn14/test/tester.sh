#!/bin/bash

# Clean previous tester files and any .o files
# rm -f test*.as test*.am test*.ob test*.ent test*.ext *.o test*.out

echo "========================================"
echo "Compiling the project..."
echo "========================================"
make

if [ $? -ne 0 ]; then
    echo "Compilation failed! Fix errors before running tests."
    exit 1
fi

if [ ! -f "./assembler" ]; then
    echo "Error: Executable 'assembler' not found. Make sure your Makefile builds it."
    exit 1
fi

echo ""
echo "========================================"
echo "Generating test cases..."
echo "========================================"

# --- VALID TESTS ---

cat <<'EOF' >test01_basic.as
; 1. Basic R-type instructions
MAIN: add $3, $5, $9
      sub $1, $2, $3
      and $4, $5, $6
      or $7, $8, $9
      nor $10, $11, $12
      hlt
EOF

cat <<'EOF' >test02_imm.as
; 2. Basic I-type instructions
MAIN: addi $3, -15, $9
      subi $1, 20, $3
      andi $4, 100, $6
      ori $7, 200, $9
      nori $10, 300, $12
      hlt
EOF

cat <<'EOF' >test03_jmp.as
; 3. Basic J-type and branching instructions
MAIN: jmp MAIN
      jmp $7
      la MAIN
      call MAIN
      bne $1, $2, MAIN
      beq $3, $4, MAIN
      blt $5, $6, MAIN
      bgt $7, $8, MAIN
      hlt
EOF

cat <<'EOF' >test04_data.as
; 4. Data directives
.entry DATA1
DATA1: .db 10, -20, 30
       .dh 1000, -2000, 3000
       .dw 100000, -200000, 300000
       .asciz "Hello Assembler!"
EOF

cat <<'EOF' >test05_macro.as
; 5. Basic macro usage
mcro DO_MATH
    add $1, $2, $3
    sub $4, $5, $6
mcroend
MAIN: DO_MATH
      hlt
EOF

cat <<'EOF' >test06_extern.as
; 6. Extern usage
.extern EXT1
.extern EXT2
MAIN: jmp EXT1
      la EXT2
      call EXT1
      hlt
EOF

cat <<'EOF' >test07_entry.as
; 7. Entry usage
.entry MAIN
.entry DATA_LBL
MAIN: add $1, $2, $3
      hlt
DATA_LBL: .db 1, 2, 3
EOF

cat <<'EOF' >test08_complex_valid.as
; 8. Complex combined usage
.entry START
.extern EXT_VAR
mcro DO_MATH
    add $1, $2, $3
    sub $4, $5, $6
mcroend
START:  DO_MATH
        move $7, $8
        mvhi $9, $10
        mvlo $11, $12
        bne $1, $2, LOOP
LOOP:   lb $9, 10, $10
        sb $11, 20, $12
        jmp EXT_VAR
        call EXT_VAR
        hlt
DATA_LBL: .db 10, -20, 30
EOF

cat <<'EOF' >test09_edge_valid.as
; 9. Edge valid case with strange spacing, tabs, and cases

    .entry   LBL

LBL:    add    $1  ,   $2 ,   $3

; This is a comment
    .extern EXT_EDGE
    jmp EXT_EDGE
EOF

cat <<'EOF' >test10_macro_advanced.as
; 10. Advanced macro with multiple lines and empty macro
mcro EMPTY_MACRO
mcroend
mcro MULTI_MACRO
    add $1, $2, $3
    sub $4, $5, $6
    move $7, $8
mcroend
MAIN: EMPTY_MACRO
      MULTI_MACRO
      hlt
EOF

# --- ERROR TESTS ---

cat <<'EOF' >test11_err_syntax.as
; 11. Syntax errors
MAIN: add $1, , $2
      sub $1 $2 $3
      move $1,
      hlt
EOF

cat <<'EOF' >test12_err_label.as
; 12. Duplicate label error
MAIN: add $1, $2, $3
MAIN: sub $1, $2, $3
      hlt
EOF

cat <<'EOF' >test13_err_undef.as
; 13. Undefined label error
MAIN: jmp UNDEF_LABEL
      bne $1, $2, UNDEF_LABEL
      hlt
EOF

cat <<'EOF' >test14_err_macro_name.as
; 14. Macro with same name as instruction
mcro add
    sub $1, $2, $3
mcroend
MAIN: add $1, $2, $3
      hlt
EOF

cat <<'EOF' >test15_err_macro_syntax.as
; 15. Extra characters on macro definition and end lines
mcro BAD_MACRO1 extra_junk
    add $1, $2, $3
mcroend
mcro BAD_MACRO2
    sub $1, $2, $3
mcroend some_garbage
MAIN: add $1, $2, $3
      hlt
EOF

cat <<'EOF' >test16_err_regs.as
; 16. Invalid register numbers
MAIN: add $32, $1, $2
      sub $-1, $2, $3
      move $100, $2
      hlt
EOF

cat <<'EOF' >test17_err_data.as
; 17. Invalid data declarations
.db 1, 2, 300
.asciz "unterminated string
.dh
EOF

cat <<'EOF' >test18_err_args.as
; 18. Too many or too few arguments
MAIN: add $1, $2
      sub $1, $2, $3, $4
      bne $1, MAIN
      hlt
EOF

cat <<'EOF' >test19_err_entry_extern.as
; 19. Both entry and extern on same label
.entry LBL
.extern LBL
LBL: add $1, $2, $3
     hlt
EOF

cat <<'EOF' >test20_err_imm.as
; 20. Immediate values out of bounds
MAIN: addi $1, 100000, $2
      subi $3, -200000, $4
      hlt
EOF

echo ""
echo "========================================"
echo "Running individual tests..."
echo "========================================"

FAILED_TESTS=0
PASSED_TESTS=0

run_test() {
    local test_name=$1
    local expect_success=$2

    echo -n "Testing ${test_name}.as... "
    ./assembler "${test_name}.as" >"${test_name}.out" 2>&1

    local ob_exists=0
    if [ -f "${test_name}.ob" ]; then
        ob_exists=1
    fi

    if [ "$expect_success" -eq 1 ]; then
        if [ $ob_exists -eq 1 ]; then
            echo "[SUCCESS]"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo "[FAILED] Expected success but no .ob file was generated."
            echo "Output of assembler:"
            cat "${test_name}.out"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    else
        if [ $ob_exists -eq 0 ]; then
            echo "[SUCCESS]"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo "[FAILED] Expected failure but .ob file WAS generated."
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    fi
}

VALID_TESTS=(
    "test01_basic"
    "test02_imm"
    "test03_jmp"
    "test04_data"
    "test05_macro"
    "test06_extern"
    "test07_entry"
    "test08_complex_valid"
    "test09_edge_valid"
    "test10_macro_advanced"
)

ERROR_TESTS=(
    "test11_err_syntax"
    "test12_err_label"
    "test13_err_undef"
    "test14_err_macro_name"
    "test15_err_macro_syntax"
    "test16_err_regs"
    "test17_err_data"
    "test18_err_args"
    "test19_err_entry_extern"
    "test20_err_imm"
)

for t in "${VALID_TESTS[@]}"; do
    run_test "$t" 1
done

for t in "${ERROR_TESTS[@]}"; do
    run_test "$t" 0
done

echo ""
echo "========================================"
echo "Running multi-file batch test..."
echo "========================================"
# Clean up any leftover outputs first to ensure clean test
rm -f test01_basic.ob test02_imm.ob test11_err_syntax.ob

./assembler test01_basic.as test11_err_syntax.as test02_imm.as >batch_test.out 2>&1

BATCH_FAILED=0
if [ ! -f "test01_basic.ob" ]; then
    echo "[FAILED] Batch test: test01_basic.ob was not generated."
    BATCH_FAILED=1
fi
if [ -f "test11_err_syntax.ob" ]; then
    echo "[FAILED] Batch test: test11_err_syntax.ob SHOULD NOT have been generated."
    BATCH_FAILED=1
fi
if [ ! -f "test02_imm.ob" ]; then
    echo "[FAILED] Batch test: test02_imm.ob was not generated. Did your program stop after the error in file 2?"
    BATCH_FAILED=1
fi

if [ $BATCH_FAILED -eq 0 ]; then
    echo "[SUCCESS] Multi-file batch processing works correctly."
else
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi

echo ""
echo "========================================"
echo "Test Summary:"
echo "Passed: $PASSED_TESTS"
echo "Failed: $FAILED_TESTS"
echo "========================================"

echo ""
echo "========================================"
echo "Cleaning up tester's junk..."
echo "========================================"
rm -f test*.as test*.am test*.ob test*.ent test*.ext test*.out batch_test.out *.o
# Uncomment the next line if you want to automatically clean .o files too
# rm -f *.o

echo "Cleanup done!"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "All tests passed successfully! 🚀"
    exit 0
else
    echo "Some tests failed. 🚨"
    exit 1
fi
