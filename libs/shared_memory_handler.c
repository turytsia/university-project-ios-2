#include"header.h"

#define FILE_KEY "main.c" 

//creates shared memory
ipc_t* ipc_init(){

    key_t key = ftok(FILE_KEY,'c');

    int ipc_key = shmget(key, sizeof(ipc_t), 0666 | IPC_CREAT);

    ipc_t* ipc = shmat(ipc_key, NULL, 0);

    ipc->shmid = ipc_key;

    return ipc;
}

//destroyes shared memory
void ipc_destroy(ipc_t*ipc){

    key_t key = ipc->shmid;

    shmctl(key, IPC_RMID, NULL);

    shmdt(ipc);
}