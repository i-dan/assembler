#ifndef FILE_MANAGEMENT_H
#define FILE_MANAGEMENT_H

#include <stdio.h>

/* -------------------------------------------------------------------------*/
/*                             file_management.c                            */
/*                     Manage file opening and closing                      */
/* -------------------------------------------------------------------------*/



/* Manage file processing flow */
void process_file(char*file_name);


/* open_file: open a file with the given name, postfix and opening mode. return the opened file */
FILE* open_file(char*file_name, char* file_postfix, char* mode);


/* create output files
 * get char* fileName and pointers to FILE* for each file - object, entry and extern*/
void create_output_files(char* file_name,FILE **ob_fd, FILE **ext_fd, FILE **ent_fd);


/* close_files: close output files / delete in case of error or irrelevant files
 * args: file name. FILE* pointers to object, entry and extern files */
void close_files(char* file_name, FILE *ob_fd, FILE *ext_fd, FILE *ent_fd);


/* get pointer to FILE and return true if it is empty, otherwise false */
int is_empty_file(FILE *fd);


/* close a given file. in case the file is empty or assembler detected and error, it is deleted.
 * Params- fileName, file postfix and FILE pointer to the open file.
 */
void close_output_file(char*file_name, char* file_postfix, FILE* fd);




/* ----------------ERRORS STATUS FLAG MANAGEMENT------------------ */
/* set_error_status: set errors status to TRUE or FALSE */
void set_error_status(char bool);

/* get_error_status: error status getter */
int get_error_status();

#endif
