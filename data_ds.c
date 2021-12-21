#include <stdlib.h>
#include "assembler.h"
#include "data.h"
#define MASK24 0xffffff



/* functions and types declarations */

typedef struct node * Node_ptr;
typedef struct node {
    unsigned long address;
    long data;
    Node_ptr next;
} Node;

static Node_ptr node_alloc();




/* declare linked list head & tail */
Node_ptr head, tail;



/* insert_data: add node to the tail of the list */
void insert_data(unsigned long dc, long data) {
    Node_ptr p;

    /* prepare node */
    if(!(p = node_alloc())) {
        printf("Error in line %lu: memory allocation failure", get_line_number());
        exit(1);
    }
    p->address = dc;
    p->data = data;
    p->next = NULL;


    if(head == NULL) {
        head = p;
        tail = p;
        return;
    }

    /* list is not empty, add p to the tail */
    tail->next = p;
    tail = p;
}


/* print_data: loop through datalist and print .data and .string address and value. */
void print_data(FILE *fd, int ic) {
    Node_ptr p;

    for(p = head; p != NULL; p = p->next) {
        fprintf(fd, "%07lu %06lx\n", p->address + ic, (p->data) & MASK24);
    }
}


/* node_alloc: allocate memory to sizeof Node. return a pointer with success, otherwise NULL */
static Node_ptr node_alloc() {
    return (Node_ptr)malloc(sizeof(Node));
}



/* free_datalist: clear datalist, free allocated memory */
void free_datalist() {
    Node_ptr p ;

    while(head) {
        p = head;
        head = head->next;
        free(p);
    }
    tail = NULL;
}



/*
void update_ic(int ic) {
    Node_ptr p;

    for(p = head; p != NULL; p = p-> next) {
        p->address += ic;
    }
}
*/