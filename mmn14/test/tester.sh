#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${YELLOW}==========================================${NC}"
echo -e "${YELLOW}   Assembler Automated Testing Suite      ${NC}"
echo -e "${YELLOW}       (With Content Verification)        ${NC}"
echo -e "${YELLOW}==========================================${NC}\n"

if [ ! -f "./assembler" ]; then
    echo -e "${RED}Error: ./assembler executable not found! Compile your C code first.${NC}"
    exit 1
fi

# ==========================================
# Phase 1: Generate Test Files
# ==========================================
echo "Generating test files..."

# --- 1. CONTENT VERIFICATION TEST ---
# We know exactly what the output of this file should be.
cat << 'EOF' > test_val_content.as
.entry MAIN
.extern EXT
MAIN: move $1, $2
      jmp EXT
      .db 10, 20
EOF

# Expected .ent file
cat << 'EOF' > test_val_content.expected.ent
MAIN 0100
EOF

# Expected .ext file
cat << 'EOF' > test_val_content.expected.ext
EXT 0104
EOF

# Expected .ob file (Based on your C bitwise logic)
# Header: 8 bytes code, 2 bytes data
# move $1, $2 -> op=1, func=1, rs=1, rd=2 -> 0x04201040 -> 40 10 20 04
# jmp EXT -> op=30, reg=0 -> 0x78000000 -> 00 00 00 78
# data -> 10, 20 -> 0A 14
cat << 'EOF' > test_val_content.expected.ob
8 2
0100 40 10 20 04
0104 00 00 00 78
0108 0A 14
EOF

# --- Standard Valid Tests ---
cat << 'EOF' > test_val_basic.as
.entry START
START: add $1, $2, $3
       hlt
EOF

cat << 'EOF' > test_val_mac.as
mcro DO_MATH
    add $5, $5, $5
mcroend
MAIN:   DO_MATH
        hlt
EOF

cat << 'EOF' > test_edge_limits.as
MIN_MAX: addi $31, 32767, $0    
         subi $0, -32768, $31   
         .db 127, -128, 0       
         hlt
EOF

cat << 'EOF' > test_edge_spaces.as
  MAIN:	        add   $1   ,  $2   ,   $3
               hlt     
EOF

# --- Error Tests ---
cat << 'EOF' > test_err_syntax.as
MAIN:   add $1, $2       ; Error: Missing operand
        .db 1, , 2       ; Error: Multiple consecutive commas
EOF

cat << 'EOF' > test_err_undef.as
MAIN:   add $1, $2, $3
        jmp GHOST_LBL    ; Error: Label doesn't exist
        hlt
EOF

cat << 'EOF' > test_err_dup.as
L1:     add $1, $2, $3
L1:     sub $4, $5, $6   ; Error: Duplicate
        hlt
EOF

cat << 'EOF' > test_err_long.as
MAIN:   add $1, $2, $3
; This line is exactly 81 characters long and should trigger an error in the code
        hlt
EOF

cat << 'EOF' > test_err_mac_res.as
mcro add
    sub $1, $2, $3
mcroend
MAIN: move $1, $2
      hlt
EOF


# ==========================================
# Phase 2: Execution & Verification
# ==========================================

PASSED=0
FAILED=0

# Helper function to check content against .expected file
check_content() {
    EXT=$1
    FILE=$2
    if [ -f "${FILE}.expected.${EXT}" ]; then
        # Use diff -w to ignore trailing whitespaces/newlines just in case
        DIFF_OUT=$(diff -w "${FILE}.${EXT}" "${FILE}.expected.${EXT}")
        if [ $? -ne 0 ]; then
            echo -e "${RED}[FAIL]${NC} - Content mismatch in .${EXT} file!"
            echo -e "${CYAN}Expected vs Actual Diff:${NC}"
            echo "$DIFF_OUT"
            return 1
        fi
    fi
    return 0
}

run_success_test() {
    FILE=$1
    echo -n "Testing $FILE ... "
    
    # Run assembler
    ./assembler $FILE > /dev/null 2>&1
    
    # 1. Check if files generated
    if [ ! -f "${FILE}.ob" ]; then
        echo -e "${RED}[FAIL]${NC} - .ob file missing!"
        FAILED=$((FAILED+1))
        return
    fi
    
    # 2. Check internal content if expected files exist
    check_content "ob" "$FILE" || { FAILED=$((FAILED+1)); return; }
    check_content "ent" "$FILE" || { FAILED=$((FAILED+1)); return; }
    check_content "ext" "$FILE" || { FAILED=$((FAILED+1)); return; }

    echo -e "${GREEN}[PASS]${NC} - Output generated & verified."
    PASSED=$((PASSED+1))
}

run_failure_test() {
    FILE=$1
    echo -n "Testing $FILE ... "
    
    rm -f "${FILE}.ob"
    ./assembler $FILE > /dev/null 2>&1
    
    if [ ! -f "${FILE}.ob" ]; then
        echo -e "${GREEN}[PASS]${NC} - Error caught successfully."
        PASSED=$((PASSED+1))
    else
        echo -e "${RED}[FAIL]${NC} - .ob file WAS generated! Error missed."
        FAILED=$((FAILED+1))
    fi
}

echo -e "\n${YELLOW}--- Running VALID & CONTENT VERIFICATION Tests ---${NC}"
run_success_test "test_val_content"   # This one checks EXACT content!
run_success_test "test_val_basic"
run_success_test "test_val_mac"
run_success_test "test_edge_limits"
run_success_test "test_edge_spaces"

echo -e "\n${YELLOW}--- Running ERROR Tests ---${NC}"
run_failure_test "test_err_syntax"
run_failure_test "test_err_undef"
run_failure_test "test_err_dup"
run_failure_test "test_err_long"
run_failure_test "test_err_mac_res"


# ==========================================
# Phase 3: Summary
# ==========================================
echo -e "\n${YELLOW}==========================================${NC}"
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}ALL $PASSED TESTS PASSED SUCCESSFULLY!${NC}"
else
    echo -e "Summary: ${GREEN}$PASSED Passed${NC} / ${RED}$FAILED Failed${NC}"
fi
echo -e "${YELLOW}==========================================${NC}"