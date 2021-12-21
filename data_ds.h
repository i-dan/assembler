#ifndef UNTITLED_DATA_DS_H
#define UNTITLED_DATA_DS_H

/* insert_data: add node to the tail of the list */
void insert_data(unsigned long dc, long data);


/* print_data: loop through datalist and print .data and .string address and value. */
void print_data(FILE *fd, int ic);


/* free_datalist: clear datalist, free allocated memory */
void free_datalist();


#endif
