#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>

#define ERROR_MSG(msg_param)\
    printf("Error in line %lu: %s\n" ,get_line_number(), msg_param); /*Print a suitable ERROR message with the line number currently in process*/


/* -------------------------------------------------------------------------*/
/*                                 assembler.h                              */
/*                   core functions for assembling a file                   */
/* -------------------------------------------------------------------------*/



/* phase_one: first pass over *.as file
 * - check for semantics errors and print a report for any error
 * - look for labels and store them in symbol_table
 * - add extern labels to symbol_table
 * - add data (integers) to data_ds
 * - add string characters to data_dc
 * - manage line_number indicator
 * - return TRUE if error has not found, otherwise FALSE
 * */
int phase_one(FILE *fd);



/* phase_two: second pass over *.as file and print all machine bitcodes into ob_fd.
 * print all extern labels references into ex
 * print entry origins into ent
 * - check for additional semantics and logical errors and print a report for any error
 * - identify entry references addresses
 * - clear tables and free all of the allocated memory
 * - manage line_number indicator
 * - if errors have not found and print made successfully return TRUE, otherwise FALSE
 * */
int phase_two(FILE *as, FILE *ob, FILE *ex, FILE *ent);




/* -------------------------------------------------------------------------*/
/*                          first phase functions                           */
/*     identify operands addressing methods and increment IC accordingly    */
/*                        report of any errors if exist                     */
/*                 return TRUE if error found otherwise FALSE               */
/* -------------------------------------------------------------------------*/


/* ic_operation1: update IC and symbol table for operation of group 1
 * s should contain consecutive strings: label (if LABEL), first operand, second operand */
int ic_operation1(char *s, int LABEL);



/* ic_operation2: update IC and symbol table for operation of group 2
* s should contain consecutive strings: label (if LABEL), operand */
int ic_operation2(char *s, int LABEL);


/* ic_operation3: update IC and symbol table for operation of group 3*/
int ic_operation3(char *s, int LABEL);



/* add_string: take care of .string instruction.
 * if LABEL = TRUE add label to symbol table with the appropriate parameters
 * add string chars to data_ds and add \0 at the end of any string
 * update DC accordingly
 * */
int add_string(char *label, char *string, int LABEL);



/* add_data: take care of .data instruction.
 * if LABEL = TRUE add label to symbol table with the appropriate parameters
 * add string chars to data_ds and add \0 at the end of any string
 * update DC accordingly
 * */
int add_data(char *label, long *data, int i, int LABEL);




/* add_extern: take care of .data instruction.
 * add label to the symbol table with the appropriate parameters
 * return TRUE with success otherwise FALSE
 * */
int add_extern(char *label);





/* -------------------------------------------------------------------------*/
/*                          second phase functions                          */
/*     identify operands addressing methods and increment IC accordingly    */
/*                        report of any errors if exist                     */
/*       extract all possible details for any operation and print its       */
/*                          bitcode words/s                                 */
/*                 return TRUE if error found otherwise FALSE               */
/* -------------------------------------------------------------------------*/


/* add_operation1: extract any details that is necessary and print bitcode word/s
 * op: index of operation from group 1 at operation_table
 * s: consecutive string contains both of the operands */
int add_operation1(char *s, int op);



/* add_operation2: extract any details that is necessary and print bitcode word/s
 * op: index of operation from group 2 at operation_table
 * s: operand */
int add_operation2(char *s, int op);



/* add_operation2: extract any details that is necessary and print bitcode word/s
 * op: index of operation from group 3 at operation_table */
int add_operation3(int op);


/* process_entry: process entry label and print its details
 * if label not found print an appropriate error message
 * return 1 with success otherwise 0*/
int process_entry(char *label);


/* get_line_number: return the number of line in process */
unsigned long get_line_number();

/* set_fd_eof: fd_eof flag setter */
void set_fd_eof(int boolean);

/* get_fd_eof: fd_eof flag getter */
int get_fd_eof();


/* get_IC : IC getter */
unsigned long get_IC();



#endif
