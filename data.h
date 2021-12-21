#ifndef DATA_H
#define DATA_H


#define IC_START 100
#define MAXLINE 81 /* 80 '\n' */
#define MAXLABEL 31
#define INSTRUCTION_TABLE_LENGTH 5
#define OPERATION_TABLE_LENGTH 17
#define GROUP_1_END_INDEX 5
#define REGISTER_TABLE_LENGTH 8
#define GROUP_2_END_INDEX 14
#define GROUP_3_END_INDEX 16



#define ASSEMBLY_POSTFIX ".as"
#define OBJ_POSTFIX ".ob"
#define EXT_POSTFIX ".ext"
#define ENT_POSTFIX ".ent"


#define MAX_DATA 16777215 /* 2^24-1 */
#define MIN_DATA -16777216 /* -(2^24) */
#define MAX_IMMEDIATE 2097151 /* 2^21 -1 */
#define MIN_IMMEDIATE -2097152 /* -(2^21) */
#define MAX_MEMORY 2097151 /* 2^21 -1 */

/*ARE bit location constants: */
#define A 4 
#define R 2
#define E 1
#define SIGNED_A 4

/* Instruction operation type: */
enum {NONE, DATA, STRING, ENTRY, EXTERN, CODE};
/* Addressing methodes: */
enum {IMMEDIATE, SYMBOL, RELATIVE, REGISTER};
/* enum {MOV=1, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, JSR, RED, PRN, RTS, STOP}; */
enum {FALSE, TRUE};


typedef struct operation {
    char name[5];
    unsigned opcode;
    unsigned func;
    unsigned src_method[4]; /* contains boolean constants. array cells corresponding to DATA=1, STRING, ENTRY, EXTERN */
    unsigned des_method[4];
} Operation;


const Operation operation_table[17];
const char instruction_table[5][8];
const char registers[8][3];

#endif
