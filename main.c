#include "file_management.h"

/* Authors:
*Dan Ivgy 208496075
*Ron Domnitz 203217286
*
*/



/* main program function - read file names from command line and send them to process.
 *Params - command line arguments*/
int main(int argc, char *argv[]){

    /* check: valid argument amount */
    if(argc < 2) {
        printf("Please run with at list one file name argument\n");
        return 1; /* exit with error */
    }

    /* send files to process */
    while (*(++argv)){
        process_file(*argv);
    }

    return 0;
}