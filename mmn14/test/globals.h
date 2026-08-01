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

/* Instruction bitfields */
typedef struct {
    unsigned int unused : 6;
    unsigned int funct : 5;
    unsigned int rd : 5;
    unsigned int rt : 5;
    unsigned int rs : 5;
    unsigned int opcode : 6;
} RType;

typedef struct {
    signed int immed : 16;
    unsigned int rt : 5;
    unsigned int rs : 5;
    unsigned int opcode : 6;
} IType;

typedef struct {
    unsigned int address : 25;
    unsigned int reg : 1;
    unsigned int opcode : 6;
} JType;

typedef union {
    RType r;
    IType i;
    JType j;
    unsigned int rawCode;
} Instruction;

typedef union {
    struct {
        unsigned int b0 : 8;
        unsigned int b1 : 8;
        unsigned int b2 : 8;
        unsigned int b3 : 8;
    } bytes;
    unsigned int rawCode;
} mCode;

/* Immediate bounds */
#define MAX_IMMED 32767
#define MIN_IMMED -32768

/* Bytes and words */
#define NUM_BYTES_WORD 4

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

#define MIN_ARGS 2

#endif
