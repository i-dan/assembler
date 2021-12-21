#include "assembler.h"
#include "symbol_table.h"
#include "parcer.h"
#include "print_functions.h"
#include "assembler_utilities.h"
#include "data.h"
#include "data_ds.h"

unsigned long line_number;
unsigned long IC, DC;
unsigned long FIC, FDC;
FILE *ob_fd, *ext_fd, *ent_fd;

/* -------------------------------------------------------------------------*/
/*                                 assembler.c                              */
/*                   core functions for assembling a file                   */
/* -------------------------------------------------------------------------*/


/* function declarations */
void free_tables();



int phase_one(FILE *fd) {
    char ERROR_FOUND = FALSE;
    set_fd_eof(FALSE);
    line_number = 1;
    IC = IC_START;
    DC = 0;

    while(!get_fd_eof()) {
        if (!process_line(fd))
            ERROR_FOUND = TRUE;
        line_number++;
    }

    if(ERROR_FOUND)
        return FALSE;

    if(IC-IC_START+DC > MAX_MEMORY) {
        printf("Error: number of orders exceeds target machine's memory limits");
        return FALSE;
    }

    FIC = IC, FDC = DC;
    update_dc(IC);

    return 1;
}


int phase_two(FILE *as, FILE *ob, FILE *ex, FILE *ent) {
    char ERROR_FOUND = FALSE;

    /* reset variables */
    line_number = 1;
    IC = IC_START;
    set_fd_eof(FALSE);

    ob_fd = ob, ext_fd = ex, ent_fd = ent;

    /* print sum of orders and data at the head of ob */
    fprintf(ob, "\t%lu %lu\n", FIC - IC_START, FDC);

    while(!get_fd_eof()) {
        if(!process_line2(as))
             ERROR_FOUND = TRUE;
        line_number++;
    }



    print_data(ob_fd, IC);

    free_tables();

    if(ERROR_FOUND)
        return FALSE;

    return TRUE;
}


int ic_operation1(char *s, int LABEL) {
    unsigned optype;


    if(LABEL) {
        if(!validate_label(s))
            return 0;
        if(!add_label(s, IC, CODE, NONE))
            return 0;
        advance_ptr(&s);
    }

    IC++; /* first bitcode of any order */

    /* identify first operand.
     * any other validation is not necessary within 1st pass */
    optype = determine_method(s);
    if(optype == IMMEDIATE || optype == SYMBOL || optype == RELATIVE)
        IC++;

    advance_ptr(&s);

    /* identify second operand */
    optype = determine_method(s);
    if(optype == IMMEDIATE || optype == SYMBOL || optype == RELATIVE)
        IC++;

    return 1;
}



/* ic_operation2: update IC and symbol table for operation of group 2
* s should contain consecutive strings: label (if LABEL), operand */
int ic_operation2(char *s, int LABEL) {
    int optype;

    if(LABEL) {
        if(!validate_label(s))
            return 0;
        if(!add_label(s, IC, CODE, NONE))
            return 0;
        advance_ptr(&s);
    }

    IC++; /* first bitcode of any order */


    /* identify first operand.
     * any other validation is not necessary within 1st pass */
    optype = determine_method(s);
    if(optype == IMMEDIATE || optype == SYMBOL || optype == RELATIVE)
        IC++;

    return 1;
}



int ic_operation3(char *s, int LABEL) {

    if(LABEL) {
        if(!validate_label(s))
            return 0;
        if(!add_label(s, IC, CODE, NONE))
            return 0;
    }

    IC++;

    return 1;
}




int add_string(char *label, char *string, int LABEL) {

    if(LABEL) {
        if(!validate_label(label))
            return 0;
        if(!add_label(label, DC, STRING, NONE))
            return 0;
    }

    while(*string) {
        insert_data(DC++, *string++);
    }
    insert_data(DC++, '\0');

    return 1;
}



int add_data(char *label, long *data, int i, int LABEL) {
    int j;

    if(LABEL) {
        if(!validate_label(label))
            return 0;
        if(!add_label(label, DC, DATA, NONE))
            return 0;
    }

    for(j = 0; j < i; j++) {
        if(data[j] > MAX_DATA || data[j] < MIN_DATA) {
            printf("Error at line %lu: data is not in allowed range", get_line_number());
            return 0;
        }
        insert_data(DC++, data[j]);
    }

    return 1;
}



int add_extern(char *label) {

    if(!validate_label(label))
        return 0;

    return (add_extern_label(label));
}






int add_operation1(char *s, int op) {
    char *s2 = s;
    unsigned op1method, op2method;
    unsigned reg1 = 0, reg2 = 0;
    unsigned long address1, address2;
    unsigned are1, are2;
    long n1, n2;


    /* identify operands type. */
    op1method = determine_method(s);

    switch(op1method) {

        case IMMEDIATE:
            if(!parse_immediate(s, &n1))
                return 0;
            are1 = A;
            break;

        case SYMBOL:
            if(!get_symbol(s, &address1, &are1))
                return 0;
            break;

        case RELATIVE:
            if(!get_relative(s, &n1))
                return 0;
            are1 = A;
            break;

        case REGISTER:
            reg1 = s[1] - '0';
            break;
    }

    advance_ptr(&s2);
    op2method = determine_method(s2);

    switch(op2method) {

        case IMMEDIATE:
            if(!parse_immediate(s2, &n2))
                return 0;
            are2 = A;
            break;

        case SYMBOL:
            if(!get_symbol(s2, &address2, &are2))
                return 0;
            break;

        case RELATIVE:
            if(!get_relative(s2, &n2))
                return 0;
            are2 = A;
            break;

        case REGISTER:
            reg2 = s2[1] - '0';
            break;
    }

    if(!validate_operands_methods(op, op1method, op2method))
        return 0;

    print_operation(ob_fd, IC, op, op1method, reg1, op2method, reg2);
    IC++;

    switch (op1method) {
        case IMMEDIATE:
            print_immediate(ob_fd, IC, n1);
            IC++;
            break;

        case SYMBOL:
            print_direct(ob_fd, IC, address1, are1);
            if(are1 == E)
                print_extern(ext_fd, s, IC);
            IC++;
            break;

        case RELATIVE:
            print_relative(ob_fd, IC, n1);
            IC++;
            break;
    }

    switch (op2method) {
        case IMMEDIATE:
            print_immediate(ob_fd, IC, n2);
            IC++;
            break;

        case SYMBOL:
            print_direct(ob_fd, IC, address2, are2);
            if(are2 == E)
                print_extern(ext_fd, s2, IC);
            IC++;
            break;

        case RELATIVE:
            print_relative(ob_fd, IC, n2);
            IC++;
            break;
    }

    return 1;
}



/* add_operation2: extract any details that is necessary and print bitcode word/s
 * op: index of operation from group 2 at operation_table
 * s: operand */
int add_operation2(char *s, int op) {
    unsigned op_method;
    unsigned reg = 0;
    unsigned long address;
    unsigned are;
    long n;

    op_method = determine_method(s);

    if(!validate_operand_method(op, op_method))
        return 0;

    switch(op_method) {

        case IMMEDIATE:
            if(!parse_immediate(s, &n))
                return 0;
            break;

        case SYMBOL:
            if(!get_symbol(s, &address, &are))
                return 0;
            break;

        case RELATIVE:
            if(!get_relative(s+1, &n)) /* +1 skip '&' */
                return 0;
            break;

        case REGISTER:
            reg = s[1] - '0';
            break;
    }


    print_operation(ob_fd, IC, op, 0, 0, op_method, reg); /* -1 since enum starts from 1 */
    IC++;

    switch (op_method) { /* ADD ADDRESS TO PRINT */
        case IMMEDIATE:
            print_immediate(ob_fd, IC, n);
            IC++;
            break;

        case SYMBOL:
            print_direct(ob_fd, IC, address, are);
            if(are == E)
                print_extern(ext_fd, s, IC);
            IC++;
            break;

        case RELATIVE:
            print_relative(ob_fd, IC, n);
            IC++;
            break;
    }

    return 1;
}



/* add_operation2: extract any details that is necessary and print bitcode word/s
 * op: index of operation from group 3 at operation_table */
int add_operation3(int op) {
    print_operation(ob_fd, IC, op, 0, 0, 0, 0);
    IC++;

    return 1;
}



int process_entry(char *label) {
    unsigned long n = add_entry_label(label);
    if(n){
        print_entry(ent_fd, label, n);
        return 1;
    }
    return 0;
}





/* clear all the tables and free all of the memory allocated for processing one file */
void free_tables() {
    free_symbol_table();
    free_datalist();
}


/* -------------------------------------------------------------------------*/
/*                        information encapsulation                         */
/* -------------------------------------------------------------------------*/


unsigned long get_line_number() {
    return line_number;
}


/*------------------------ fd_eof() flag management ------------------------*/
static int FD_EOF;

void set_fd_eof(int boolean) {
    FD_EOF = boolean;
}

int get_fd_eof() {
    return FD_EOF;
}


/* get_IC : IC getter */
unsigned long get_IC() {
    return IC;
}


/*------------------------ extern_instruction_exist() flag management ------------------------*/

static char extern_instruction_flag;

/* set_extern_flag: extern instruction flag setter */
void set_extern_flag(char bool)     { extern_instruction_flag = bool; }

/* extern_instruction_exist: extern instruction flag getter */
char extern_instruction_exist()     { return extern_instruction_flag; }




/*
// is_last_line : return TRUE if address is the last line to print other wise FALSE
int is_last_line(unsigned long address) {
    return address == FIC + FDC -1;
}




FILE *get_ob_file() {
    return ob_fd;
}



FILE *get_ext_file() {
    return ext_fd;
}



FILE *get_ent_file() {
    return ent_fd;
}

*/





