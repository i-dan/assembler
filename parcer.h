#ifndef PARCER_H
#define PARCER_H


#include <stdio.h>




/* process_line: read a line from fd and parse it into tokens. identify if there's a label definition, determine
 * whether the line contains operation or instruction. whether it's a comment / white line  and what type of it.
 * pass the information to assembler.c functions that would handle it for additional process and to update IC.
 * check for some errors (mainly semantics errors) as a preliminary validation.
 * if the stream input got to EOF set an appropriate flag (via set_fd_eof() )
 * return 0 if error was found otherwise 1 */
int process_line(FILE *fd);


/* process_line2: used at the second pass of assembler  over the file.
 * read a line from fd. determine whether it's a comment/white line, operation or instruction.
 * determine the number of operation / instruction this line represents
 * parse it to tokens. pass the information to assembler.c functions for additional and final process;
 * additional process.
 * if the stream input got to EOF set an appropriate flag (via set_fd_eof() )
 * return 0 if error was found otherwise 1 */
int process_line2(FILE * fd);

#endif
