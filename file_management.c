#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "file_management.h"
#include "assembler.h"


/* -------------------------------------------------------------------------*/
/*                             file_management.c                            */
/*                     Manage file opening and closing                      */
/* -------------------------------------------------------------------------*/

#define MAXPATH 100 /* Length of the file name */

/* Manage file processing flow */
void process_file(char*file_name){
    FILE *asFd, *ob_fd, *ent_fd , *ext_fd;

    /* open a file to assemble */
    asFd = open_file(file_name,ASSEMBLY_POSTFIX,"r");

    if(asFd == NULL)
        return;

    /*send assembly file to first read processing*/
    printf("Start processing \"%s%s\" file.\n",file_name,ASSEMBLY_POSTFIX);

    if(!(phase_one(asFd))) {
        /*In case of error finish here*/
        fclose(asFd);
        printf("Failed to process \"%s%s\" file.\n",file_name,ASSEMBLY_POSTFIX);
        return;
    }

    /* create output files and run second read */
    create_output_files(file_name, &ob_fd,&ext_fd, &ent_fd);

    /* return -IO_read_ptr to the beginning of the file and send it to phase two */
    rewind(asFd);
    if(!phase_two(asFd, ob_fd,ext_fd,ent_fd)) {
        set_error_status(TRUE);
        printf("Failed to process \"%s%s\" file.\n",file_name,ASSEMBLY_POSTFIX);
    }

    /* close (& delete irrelevant) output files */
    fclose(asFd);
    close_files(file_name,ob_fd,ext_fd,ent_fd);
    printf("Finish processing \"%s%s\" file %s.\n\n",file_name,ASSEMBLY_POSTFIX,(get_error_status()? "with errors" : "successfully"));
}




FILE* open_file(char*file_name, char* file_postfix, char* mode){
    FILE *fd;
    char full_file_name[MAXPATH];  /*strlen(file_name) + strlen(file_postfix) + 1*/

    /* concatenate file name and postfix */
    sprintf(full_file_name, "%s%s", file_name, file_postfix);


    if(!(fd = fopen(full_file_name,mode))){
        fprintf(stdout,"Failed to open file %s\n",full_file_name); /* return fd = NULL */
    }

    return fd;
}



void create_output_files(char* file_name,FILE **ob_fd, FILE **ext_fd, FILE **ent_fd){
    /*create object, entry and extern files */
    *ob_fd = open_file(file_name,OBJ_POSTFIX,"w+");
    *ext_fd = open_file(file_name,EXT_POSTFIX,"w+");
    *ent_fd = open_file(file_name,ENT_POSTFIX,"w+");

    if(ob_fd == NULL || ext_fd == NULL || ent_fd == NULL) {
        printf("Output files creation failure. exit");
        exit(1);
    }
}



void close_files(char* file_name, FILE *ob_fd, FILE *ext_fd, FILE *ent_fd){
    /*create object, entry and extern files */
    close_output_file(file_name,OBJ_POSTFIX,ob_fd);
    close_output_file(file_name,EXT_POSTFIX,ext_fd);
    close_output_file(file_name,ENT_POSTFIX,ent_fd);
}



int is_empty_file(FILE *fd){
    int c;
    rewind(fd);     /* initialize fd to point to first position */

    if ((c = fgetc(fd)) == EOF)
        return TRUE;/* file empty, error handling */
    else
        ungetc(c, fd);
    return FALSE;
}



void close_output_file(char*file_name, char* file_postfix, FILE* fd){
    int is_empty;

    /* validate file */
    if(!fd){
        fprintf(stdout,"No file to close.\n");
        return;
    }

    is_empty = is_empty_file(fd); /* check if file is empty */
    fclose(fd); /* close file */

    /* remove empty files */
    if(is_empty || get_error_status()) {
        char full_file_name [MAXPATH];   /*strlen(file_name) + strlen(file_postfix) + 1*/

        sprintf(full_file_name, "%s%s", file_name, file_postfix);

        if(remove(full_file_name))
            fprintf(stdout,"Failed to delete file %s\n",full_file_name);
    }
}



/* ----------------ERRORS STATUS FLAG MANAGEMENT------------------ */

static char ERROR;

void set_error_status(char bool) {
    ERROR = bool;
}

int get_error_status() {
    return ERROR;
}


