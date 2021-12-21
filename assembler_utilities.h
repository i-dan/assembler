#ifndef ASSEMBLER_UTILITIES_H
#define ASSEMBLER_UTILITIES_H



/* -------------------------------------------------------------------------*/
/*                   utilities and validations for assembler                */
/* -------------------------------------------------------------------------*/

/* determine_method: get an operand string. return its addressing method */
unsigned determine_method(char *s);

/* validate_label: validate label semantics.
 * - assume label is not an empty string
 * - doe's NOT check label's length (this's parcer's role) */
int validate_label(char *label);


/* validate_operands_methods:
op: index of operation from group 1 at operation_table
method1: addressing method of the first operand
method2: addressing method of the second operand */
int validate_operands_methods(int op, int method1, int method2);


/* validate_operand_method:
op: index of operation from group 2 at operation_table
op_method: its addressing method */
int validate_operand_method(int op, int op_method);



/* parse_immediate: extract the right value for operand with immediate addressing method into n */
int parse_immediate(char *s, long *n);


/* get_symbol: get label's details, put label's address and 'are' (external / internal)
 * into address and are respectivley
 * return TRUE if label exist, otherwise FALSE */
int get_symbol(char *label, unsigned long *address, unsigned *are);



/* get_relative: compute label's interval from IC and put it in value
 * return TRUE if label found otherwise FALSE */
int get_relative(char *label, long *value);



/* advance_ptr: advance s to point on the next byte after \0 (to the next string..) */
void advance_ptr(char **s);
#endif
