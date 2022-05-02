#include "header.h"

#define init(name,value)(sem_open(name, O_CREAT | O_WRONLY, 0666, value))

void sem_create(semaphores* sem)
{
    if ((sem->sem_hydrogen = init(SEMAPHORE_HYDROGEN,0)) == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((sem->sem_oxygen = init(SEMAPHORE_OXYGEN,1)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((sem->sem_output = init(SEMAPHORE_OUTPUT,1)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((sem->sem_ready = init(SEMAPHORE_READY,0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((sem->sem_created = init(SEMAPHORE_CREATE,0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((sem->sem_creating = init(SEMAPHORE_CREATING,0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((sem->sem_stop_extra = init(SEMAPHORE_STOP_EXTRA,0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    sem_unlink(SEMAPHORE_HYDROGEN);

    sem_unlink(SEMAPHORE_OXYGEN);

    sem_unlink(SEMAPHORE_OUTPUT);

    sem_unlink(SEMAPHORE_READY);

    sem_unlink(SEMAPHORE_CREATE);

    sem_unlink(SEMAPHORE_CREATING);

    sem_unlink(SEMAPHORE_STOP_EXTRA);
}

void sem_cleanup(semaphores*sem){

    sem_close(sem->sem_hydrogen);

    sem_close(sem->sem_oxygen);

    sem_close(sem->sem_output);

    sem_close(sem->sem_ready);

    sem_close(sem->sem_created);

    sem_close(sem->sem_creating);

    sem_close(sem->sem_stop_extra);
}