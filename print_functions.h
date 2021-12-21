#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H



/*--------------------functions for printing bitcode words----------------------*/


/* print_operation: print operation bitcode. params are all the needded values. ass written */
void print_operation(FILE* fd, unsigned long ic, unsigned op, unsigned src_m, unsigned src_reg, unsigned des_m, unsigned des_reg);

/* print_direct: print bitcode of direct addressing method */
void print_direct(FILE* fd, unsigned long ic, unsigned long value, unsigned are);


/* print_relative: print bitcode of relative addressing method */
void print_relative(FILE* fd, unsigned long ic, long interval);

/* print_direct: print bitcode of immediate addressing method */
void print_immediate(FILE* fd, unsigned long ic, long value);


/*--------------functions to print label name and address for extern and entry files---------------*/


/* print_extern: print a label name and address when an extern label is used */
void print_extern(FILE *fd, char * name, unsigned long address);

/* print_entry: print a label name and address of an entry label */
void print_entry(FILE *fd, char * name, unsigned long address);



#endif
