#include "assembler.h"
#include "data.h"
#include <string.h>
#include <stdlib.h>


typedef struct label * Label_ptr;
typedef struct label {
    char *name;
    unsigned long address;
    int label_type;
    int label_prop;
    Label_ptr next;
} Label;



static char * str_dup(char *s);
static Label_ptr lalloc();
static Label_ptr search(char *s);




/* data structure based on 26 cells table, any cell point to a linked list with its corresponding
 * alphabetic letter */
Label_ptr table[26]; /* all global variable initialized to NULL */



int add_label(char *name, unsigned address, int label_type, int label_prop) {

    int i;
    char *temp;
    Label_ptr t;



    /* add that label to the symbol table*/
    if (!(temp = str_dup(name)) || !(t = lalloc())) {
        ERROR_MSG("memory allocation failure")
        exit(1);
    }

    t->name = temp, t->address = address, t->label_prop = label_prop, t->label_type= label_type;
    i = (*temp < 91) ? *temp - 65 : *temp - 97;

    if(table[i] == NULL) {
        t->next = NULL;
        table[i] = t;
        return 1;
    }

    /* table[i] != NULL */
    else {
        int cmp;
        Label_ptr p = table[i];

        /* check: t <= table[i] */
        if((cmp = strcmp(temp, p->name)) <= 0) {
            if(cmp < 0) {
                t->next = p;
                table[i] = t;
                return 1;
            }

            else { /* cmp == 0 */
                ERROR_MSG("multiple label declaration")
                return 0;
            }
        }

        /* find the place for t */
        while(p->next != NULL && (cmp = strcmp(temp, p->next->name)) > 0) {
            p = p->next;
        }

        if (p != NULL && (cmp == 0)) { /* loop stopped because name == p->next->name */
            printf("Error at line %lu: multiple label declaration\n", get_line_number());
            return 0;
        }

        t->next = p->next;
        p->next = t;

        return 1;
    }
}



/* add_extern_label: */
int add_extern_label(char *name) {
    Label_ptr p;

    if(!add_label(name, 0, NONE, EXTERN)) {
        /* add_label returned false. check why */
        p = search(name);
        if(p->label_prop != EXTERN) {
            ERROR_MSG(".extern instruction to existing internal label")
            return 0;
        }

    }

    return 1; /* add_label returned true | duplicate extern declarations */
}



unsigned long add_entry_label(char *name) { /* use in 2nd pass */
    Label_ptr t;

    if ((t = search(name))) { /* there's a label with the same name */
        if (t->label_prop == EXTERN) {
            ERROR_MSG("try to add entry label to extern label")
            return 0;
        }
        t->label_prop = ENTRY;
        return t->address;
    }

    else { /* entry instruction at the 2nd pass without having existing label */
        ERROR_MSG("entry label has not found")
        return 0;
    }

 } /* we could have merge this function with add_label and save some runtime (in case of label exist)
          but we prefer to keep it that way to ease maintenance and control the algorithm */



/* search: search for a label with s name. return Label_ptr to that label, if not found NULL */
static Label_ptr search(char *s) {
    int i;
    Label_ptr p = table[(*s < 91) ? *s - 65 : *s - 97];

    while(p != NULL && (i = strcmp(p->name, s)) < 0)
        p = p->next;

    return (i == 0)? p : NULL;
}



/* update_dc: update address of all DATA and STRING labels (add ic to their current address) */
void update_dc(unsigned long ic) {
    int i;
    Label_ptr p;

    for(i = 0; i < 26; i++)
    {
        p = table[i];

        while(p) {
            if(p->label_type == DATA || p->label_type == STRING)
                p->address += ic;
            p = p-> next;
        }
    }
}



/* get_address: return the address of the label with that name, if not found, return 0 */
unsigned long get_address(char *name) { /* use after 2nd pass */
    Label_ptr p = search(name);

    if (p == NULL) /* name has not found */
        return 0;

    return p->address;
}




/* get_details: return 1 of label found, otherwise 0. retrieve label's address and label_prop
 * into address and property parameters */
int get_details(char *name, unsigned long *address, int *property) {

    Label_ptr p = search(name);

    if (p == NULL) /* name has not found */
        return 0;

    *address = p->address;
    *property = p->label_prop;

    return 1;
}



/* free_symbol_table: clear the table, free allocated memory */
void free_symbol_table() {
    int i;
    Label_ptr p, q;

    for(i = 0; i < 26; i++) {
        p = table[i];
        while(p) {
            q = p;
            p = p->next;

            free(q->name);
            free(q);
        }
        table[i] = NULL;
    }
}



static char * str_dup(char *s) { /* make a duplicate of s. return p = NULL if malloc failed */
    char *p = NULL;

    p = (char *) malloc(strlen(s)+1); /* +1 for '/0' */
    if (p != NULL)
        strcpy(p, s);

    return p;
}



/* lalloc: allocate memory for label struct and return Label_ptr to it. NULL if malloc failed */
static Label_ptr lalloc() {
    return (Label_ptr) malloc(sizeof(Label));
}

