#include "data.h"
#include <stdio.h>
#define MASK24 0xffffff

/* -------------------------------------------------------------------------*/
/*                             print_functions.c                            */
/* -------------------------------------------------------------------------*/


void print_operation(FILE* fd, unsigned long ic, unsigned op, unsigned src_m, unsigned src_reg, unsigned des_m, unsigned des_reg) {

    unsigned long bitcode;

    bitcode = (operation_table[op].opcode);
    bitcode <<= 2;
    bitcode |= (src_m);
    bitcode <<= 3;
    bitcode |= src_reg;
    bitcode <<= 2;
    bitcode |= des_m;
    bitcode <<= 3;
    bitcode |= des_reg;
    bitcode <<= 5;
    bitcode |= operation_table[op].func;
    bitcode <<= 3;
    bitcode |= A;


    fprintf(fd, "%07lu %06lx\n", ic, bitcode);
}


void print_direct(FILE* fd, unsigned long ic, unsigned long value, unsigned are) {

    value <<= 3;

    if(are == E)
        value |= E;
    else
        value |= R;

    fprintf(fd, "%07lu %06lx\n", ic, value);
}



void print_relative(FILE* fd, unsigned long ic, long interval) {

    interval <<= 3;
    interval |= SIGNED_A;


    fprintf(fd, "%07lu %06lx\n", ic,  interval & MASK24);
}



/* same as print_relative, yet should write this function to keep the code clear and to ease maintenance */
void print_immediate(FILE* fd, unsigned long ic, long value) {

    value <<= 3;
    value |= SIGNED_A;



    fprintf(fd, "%07lu %06lx\n", ic, value & MASK24);

}



/*--------------functions to print label name and address for extern and entry files---------------*/



void print_extern(FILE *fd, char * name, unsigned long address) {

    fprintf(fd, "%s %07lu\n", name, address);
}



void print_entry(FILE *fd, char * name, unsigned long address) {

    fprintf(fd, "%s %07lu\n", name, address);
}



