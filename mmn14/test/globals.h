/*
 * globals.h
 * mmn14
 * Tzur Pinto Lazar
*/


#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LINE_LENGTH 256
#define MAX_LABEL_LENGTH 32
#define IC_INIT 100
#define REG_COUNT 32

typedef enum { FALSE = 0, TRUE = 1 } boolean;

typedef enum { R_TYPE, I_TYPE, J_TYPE } InstType;

/* Constants for limits and string allocations */
#define MAX_AS_LINE_LEN 80
#define MAX_LABEL_LEN 31
#define EXTRA_CHARS 2

/* Shifts for instruction fields */
#define OPCODE_POS 26
#define RS_POS 21
#define RT_POS 16
#define RD_POS 11
#define FUNCT_POS 6
#define REGBIT_POS 25

/* Bit sizes for masking */
#define OPCODE_SIZE 6
#define IMMED_SIZE 16
#define ADDR_SIZE 25
#define BYTE_SIZE 8

/* Immediate bounds */
#define MAX_IMMED 32767
#define MIN_IMMED -32768

/* Bytes and words */
#define NUM_BYTES_WORD 4
#define BYTE1 8
#define BYTE2 16
#define BYTE3 24

/* Opcode ranges based on instruction groups */
#define MIN_ARITH_OPCODE 10
#define MAX_ARITH_OPCODE 14
#define MIN_BRANCH_OPCODE 15
#define MAX_BRANCH_OPCODE 18
#define MIN_MEM_OPCODE 19
#define MAX_MEM_OPCODE 24
#define MIN_JMP_OPCODE 30
#define MAX_JMP_OPCODE 32
#define LA_OPCODE 31
#define CALL_OPCODE 32
#define STOP_OPCODE 63

/* Funct codes */
#define FUNCT_ADD 1
#define FUNCT_SUB 2
#define FUNCT_AND 3
#define FUNCT_OR 4
#define FUNCT_NOR 5
#define FUNCT_MOVE 1
#define FUNCT_MVHI 2
#define FUNCT_MVLO 3

#define BASE_10 10
#define MIN_ARGS 2

#endif
