#include "assembler.h"
#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




int process_line(FILE *fd) {

    char line[MAXLINE + 1]; /* +1 for '\0'. line role: hold the whole input line */
    char buffer[MAXLINE +1]; /* buffer role: hold the clean tokens for the next functions */
    long data[MAXLINE -6];

    register char *lp; /* line pointer, run through line with */
    register char *bp; /* buffer pointer */
    int i, n, m, token, group, ins, op;
    char c;
    int LABEL = FALSE;
    lp = line;
    bp = buffer;
    c = ins = op = 0;

    memset(line, 0, sizeof(line));
    memset(buffer, 0, sizeof(buffer));



    line[MAXLINE] = '\n'; /* use as a flag to check if fgets has not read the whole line */

    if(fgets(line, MAXLINE + 1, fd) == NULL) {
        if (feof(fd)) {
            set_fd_eof(TRUE);
            return 1;
        }
        else { /* ferror() == TRUE */
            ERROR_MSG("file reading error");
            fclose(fd);
            exit(1);
        }
    }
    if(feof(fd))
        set_fd_eof(TRUE);


    while(isspace(*lp))
        lp++;

    /* check: comment line / white line */
    if(*lp == '\0' || *lp == ';') {
        if(line[MAXLINE] != '\n')      /* line exceeds MAXLINE. it's ok with comment line. finish the line */
            while (fgetc(fd) != '\n')
                ;
        return 1;
    }


    /* check: line length */
    if(line[MAXLINE] != '\n') {
        ERROR_MSG("line exceeds the allowed length");
        while(fgetc(fd) != '\n')
            ;
        return 0;
    }


    /* extract label | operation | instruction */
    lp = line; /* redundant */
    sscanf(lp, " %n%s%n", &n, bp, &m);
    lp += m;
    token = m-n;

    /* determine whether it's a label */
    if(bp[token-1] == ':')
    {
        LABEL = TRUE;
        bp[token-1] = '\0'; /* replace ':' with '\0' */
        if(token > MAXLABEL+1) { /* +1 for ':' */
            ERROR_MSG("label exceeds the allowed length or unknown operation");
            return 0;
        }

        /* extract operation | instruction */
        bp += token;
        if(sscanf(lp, " %s%n", bp, &m) < 1) {
            ERROR_MSG("There must be an operation\\instruction after label declaration");
            return 0;
        }
        lp += m;
    }

    /* so far bp point to the first char of instruction | operation */
    /* determine weather it's operation or instruction */
    for(i = 1; i <  INSTRUCTION_TABLE_LENGTH; i++) {
        if(strcmp(bp, instruction_table[i]) == 0) {
            ins = i;
            break;
        }
    }

    if(!ins) { /* it's not instruction, look for operation */
        for(i = 1; i < OPERATION_TABLE_LENGTH; i++) {
            if (strcmp(bp, operation_table[i].name) == 0) {
                op = i;
                break;
            }
        }
    }

    /* check: first token is operation / instruction */
    if(!ins && !op) {
        ERROR_MSG("unknown operation");
        return 0;
    }


    switch(ins) {
        case DATA:
            i = 0;

            /* extract first data element */
            m = sscanf(lp, " %ld%n", &data[i++], &n);
            if(m < 1) {
                ERROR_MSG(".data instruction must be followed by integer");
                return 0;
            }
            lp += n;

            /* check: is there any char after first data element */
            c = 0;
            sscanf(lp, " %c %n", &c, &n);
            lp += n;

            /* extract next element, if exist */
            while(c != 0) {
                if(c != ',') {
                    ERROR_MSG("data element followed by irrelevant token");
                    return 0;
                }

                /* extract next data element */
                m = sscanf(lp, " %ld%n", &data[i++], &n);
                if(m < 1) {
                    ERROR_MSG("',' must be followed by an integer");
                    return 0;
                }
                lp += n;

                c = 0;
                sscanf(lp, " %c %n", &c, &n);
                lp += n;
            }
            return add_data(buffer, data, i, LABEL);


        case STRING:

            /* extract first char after ".string" */
            c = 0;
            sscanf(lp, " %c%n",  &c, &n);
            if(c == 0) {
                ERROR_MSG("missing operand after .string instruction");
                return 0;
            }
            else if(c != '\"') {
                ERROR_MSG("irrelevant token after .string instruction");
                return 0;
            }
            lp += n; /* now lp points to the beginning of the string */

            /* find the last '\"' and replace it with '\0' (notice: 'recycle' bp with a different role) */
            bp = strrchr(lp, '\"');
            if(bp == NULL) {
                ERROR_MSG("string is missing right quotes");
                return 0;
            }
            *bp = '\0';

            /* check: is there any other token after right quotes */
            c = 0;
            sscanf(bp+1, " %c ", &c);
            if(c != 0) {
                ERROR_MSG("irrelevant token after string operand");
                return 0;
            }
            return add_string(buffer, lp, LABEL);

        case EXTERN:
            if((m = sscanf(lp, " %s %c", bp, &c)) != 1) {/* sscanf read more than 1 token */
                if(m < 1) {
                    ERROR_MSG(".extern instruction must be followed by a label");
                    return 0;
                }
                /* else: m > 1 */
                ERROR_MSG("irrelevant token after extern label");
                return 0;
            }

            /* it's parcer's role to validate label's length */
            if(strlen(bp) > MAXLABEL) {
                ERROR_MSG("extern label exceeds max length");
                return 0;
            }

            return add_extern(bp);


        case ENTRY:
            c = 0;
            if((m = sscanf(lp, " %s %c", bp, &c)) > 1) { /* sscanf read more than 1 token */
                ERROR_MSG("irrelevant token after entry label");
                return 0;
            }

             /* if(strlen(bp) > MAXLBAEL) {
                    printf("Error at line %d: entry label exceeds max length", line_num); return 0; }
                we don't need this check, since if it's not a legal one, it will not find it on symbol table
                at the 2nd pass */

            return 1; /* wait for second pass */
    }

    /* determine a group to assign operation to */
    group = (op <= GROUP_1_END_INDEX)? 1 : (op <= GROUP_2_END_INDEX)? 2 : 3;

    switch(group) {
        case 1:
            /* exract first operand */
            if(sscanf(lp, " %n%[^ \t,\n]%n", &n, bp, &m) < 1) { /* sscanf has read nothing */
                ERROR_MSG("missing operands");
                return 0;
            }
            lp += m;
            token = m-n;
            bp += token+1;

            if((m = sscanf(lp, " , %s %c", bp, &c)) != 1) {
                if(m < 1) {
                    ERROR_MSG("missing operand or delimiter (',')");
                    return 0;
                }
                /* m > 1 */
                ERROR_MSG("unknown token after second operand");
                return 0;
            }

            return ic_operation1(buffer, LABEL);

        case 2:
            if ((m = sscanf(lp, " %s %c", bp, &c)) != 1) {
                if(m < 1) {
                    ERROR_MSG("missing operand");
                    return 0;
                }
                /* m > 1 */
                ERROR_MSG("unknown token after operand");
                return 0;
            }

            return ic_operation2(buffer, LABEL);

        case 3:
            /* check: is there any redundant token after operation */
            if(sscanf(lp, " %c ", &c) > 0) {
                ERROR_MSG("unknown token");
                return 0;
            }

            return ic_operation3(buffer, LABEL);
    }

    return 0; /* function should never get here */
}





int process_line2(FILE * fd) {

    char line[MAXLINE + 1]; /* +1 for '\0'  */
    char buffer[MAXLINE +1];

    register char *bp = buffer, *lp = line;
    int i, n, m, token, group, ins = 0, op = 0;

    if(fgets(line, MAXLINE, fd) == NULL) {
        if(feof(fd)) {
            set_fd_eof(TRUE);
            return 1;
        }
        else {  /* ferror() == TRUE */
            ERROR_MSG("file reading error")
            fclose(fd);
            exit(1);
        }
    }
    if(feof(fd))
        set_fd_eof(TRUE);



    /* check for comment / white line */
    while(isspace(*lp))
        lp++;
    if(*lp == '\0' || *lp == ';') /* it's a comment / white line */
        return 1;


    /* extract label | operation | instruction */
    lp = line;
    sscanf(lp, " %n%s%n", &n, bp, &m);
    lp += m;
    token = m-n;

    /* determine whether it's a label */
    if(bp[token-1] == ':')
    {
        /* skip label, extract operation | instruction */
        sscanf(lp, " %n%s%n",&n, bp, &m);
        lp += m;
        token = m-n;
    }

    /* so far bp point to the first char of instruction | operation */
    /* determine weather it's operation or instruction */
    for(i = 1; i < INSTRUCTION_TABLE_LENGTH; i++) {
        if(strcmp(bp, instruction_table[i]) == 0) {
            ins = i;
            break;
        }
    }

    if(!ins) { /* it's not instruction, look for operation */
        for(i = 1; i < OPERATION_TABLE_LENGTH; i++) {
            if(strcmp(bp, operation_table[i].name) == 0) {
                op = i;
                break;
            }
        }
    }

    bp = buffer; /* so far we have operation | instruction details. extract operands into buffer */

    switch(ins) {
        case DATA:
        case STRING:
        case EXTERN:
            return 1;

        case ENTRY:
            sscanf(lp, " %s", bp);
            return process_entry(bp);
    }

    /* determine a group to assign operation to */
    group = (op <= GROUP_1_END_INDEX)? 1 : (op <= GROUP_2_END_INDEX)? 2 : 3;

    switch(group){
        case 1:
            sscanf(lp, " %n%[^ \t,]%n", &n, bp, &m); /* extract first operand into buffer */
            lp += m;
            token = m-n;
            bp += token+1;

            sscanf(lp, " , %s", bp); /* extract first operand */

            return add_operation1(buffer, op);

        case 2:
            sscanf(lp, " %s", buffer);
            return add_operation2(buffer, op);

        case 3:
            return add_operation3(op);
    }

    return 0; /* function should never get here */

}






















