
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/*
 * name: label name. assume it's legal and in legal length
 * address: label address
 * int lab_type: expected values: CODE = 5, DATA = 1, STRING = 2, NONE = 0 (in case of extern label)
 * int lab_prop: expected values: CODE = 5, EXTERN = 2 (ENTRY has a different function below)
 * NOTICE: there's a different function right below to add an extern label, that function use this one.
 * assume all parameter were sent properly. DOES NOT valid parameter appropriateness
 * return 1 with success, otherwise 0 and print an appropriate message
 * */
int add_label(char *name, unsigned address, int label_type, int label_prop);


/* add_extern_label: add a label with property extern.
 * return 1 with success otherwise 0
 * print an appropriate message in case of failure
 * */
int add_extern_label(char *name);


/* add_entry_label: add entry property to existing label. if label doesn't exist */
unsigned long add_entry_label(char *name);


/* update_dc: update address of all DATA and STRING labels (add ic to their current address) */
void update_dc(unsigned long ic);


/* get_address: return the address of the label with that name, if not found, return 0 */
unsigned long get_address(char *name);


/* get_details: return 1 of label found, otherwise 0. retrieve label's address and label_prop
 * into address and property parameters */
int get_details(char *name, unsigned long *address, int *property);



/* free_symbol_table: clear the table, free allocated memory */
void free_symbol_table();


#endif
