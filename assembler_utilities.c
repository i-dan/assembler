#include "assembler.h"
#include "data.h"
#include "assembler_utilities.h"
#include "symbol_table.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



unsigned determine_method(char *s) {
    int i;

    if(*s == '#') {
        return IMMEDIATE;
    }

    if(*s == '&')
        return RELATIVE;

    for(i = 0; i < 8; i++) {
        if (strcmp(s, registers[i]) == 0)
            return REGISTER;
    }

    return SYMBOL;
}



int validate_label(char *label) {

    int i = 0;

    if(!isalpha(label[i++])) { /* check: first char is a letter */
        ERROR_MSG("label starts with non-alpha letter")
        return 0;
    }

    while(label[i]) { /* check: other characters are letters or digits */
        if(!isalnum(label[i])) {
            ERROR_MSG("label contains illegal character")
            return 0;
        }
        i++;
    }

    /* check: label is not a reserved word */
    for(i = 1; i < INSTRUCTION_TABLE_LENGTH; i++) {
        if(strcmp(label, instruction_table[i]+1) == 0) {    /* +1 to skip '.' */
            ERROR_MSG("label name is reserved word")
        return 0;
        }
    }
    for(i = 1; i < OPERATION_TABLE_LENGTH; i++) {
        if(strcmp(label, operation_table[i].name) == 0){
            ERROR_MSG("label name is reserved word")
            return 0;
        }
    }
    for(i = 0; i < REGISTER_TABLE_LENGTH; i++) {
        if (strcmp(label, registers[i]) == 0){
            ERROR_MSG("label name is reserved word")
            return 0;
        }
    }

    return 1;
}



int validate_operands_methods(int op, int method1, int method2) {
    if (!(operation_table[op].src_method[method1] && operation_table[op].des_method[method2])){
        ERROR_MSG("incompatible operands")
        return 0;
    }
    return 1;
}



int validate_operand_method(int op, int op_method) {
    if(!(operation_table[op].des_method[op_method])) {
        ERROR_MSG("incompatible operands")
        return 0;
    }
    return 1;
}



int parse_immediate(char *s, long *n) {
    char c;
    int m;

    /* extract element into n and check there's not any additional token after */
    if((m = sscanf(s,"#%ld %c", n, &c)) > 1) {
        ERROR_MSG("illegal token after '#' operand")
        return 0;
    }

    else if(m < 1) { /* sscanf has'nt read any number */
        ERROR_MSG("can't read number. '#' must be followed by adjacent interger")
        return 0;
    }


    if((*n < MIN_IMMEDIATE) || (*n > MAX_IMMEDIATE)) {
        ERROR_MSG("immediate integer exceeds the allowed range")
        return 0;
    }

    return 1;

}



int get_symbol(char *label, unsigned long *address, unsigned *are) {
    int x;

    if(!get_details(label, address, &x)) {
        ERROR_MSG("label has not found")
        return 0;
    }

    *are = (x != EXTERN)? R : E;
    return 1;
}



int get_relative(char *label, long *value) {
    unsigned long i;

    if(!(i = get_address(label))) {
        ERROR_MSG("label has not found")
        return 0;
    }

    *value = i - get_IC();

    return 1;
}


void advance_ptr(char **s) {
    while(*(*s)++)
        ;
}

