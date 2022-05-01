#include "header.h"

void err(int err_id){
    if(err_id == 0){
        return;
    }

    switch(err_id){
        case ERR_ARGS:
            fprintf(stderr,"ERROR: Invalud input arguments\n");
            break;
        default:
            fprintf(stderr,"ERROR: Unknown error id\n");
            break;
    }

    exit(EXIT_FAILURE);
}