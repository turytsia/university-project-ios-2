#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <semaphore.h>

#define ERR_ARGS 10

#define FILE_NAME "./proj2.out"
#define FILE_MODE "a"

#define H_t 'H'
#define O_t 'O'

#define ATOM_CREATED 20
#define ATOM_QUEUED  21
#define ATOM_DEQUEUED 22
#define ATOM_DONE 23
#define ATOM_LACK_H 24
#define ATOM_LACK_O 25

#define ATOM_CREATE(id, type)((atom_t){id,type})

#define PROCESS_CREATE(id,atom)((process_t){id,atom,0})

#define RANDOM_INTERVAL(n) (rand() % n)

void err(int);

typedef struct{
    uint NH;
    uint NO;
    uint TI;
    uint TB;
    uint EXPECTED_H;
    uint EXPECTED_O;
}args;

int read_args(args *, int , char **);

typedef struct{
    id_t id;
    char type;
}atom_t;

typedef struct{
    pid_t id;
    atom_t atom; 
    uint extra;
}process_t;

//
#define IPC_KEY 90

#define IPC_ALLOCATE(key, type)(shmget(key, sizeof(type), 0666 | IPC_CREAT))
#define IPC_AT(segment_id)(shmat(segment_id, NULL, 0))

typedef struct{
uint line_n;
uint hydrogen_n;
uint oxygen_n;
uint molecule_counter_h;
id_t molecule_id;
}ipc_t;

//

// semaphore constants
#define SEMAPHORE_HYDROGEN "semaphore_hydrogen"
#define SEMAPHORE_OXYGEN "semaphore_oxygen"
#define SEMAPHORE_OUTPUT "semaphore_output"
#define SEMAPHORE_READY "semaphore_ready"
#define SEMAPHORE_CREATE "semaphore_create"
#define SEMAPHORE_STOP_EXTRA "semaphore_stop_extra"
#define SEMAPHORE_CREATING "semaphore_creating"
#define SEM_FLAGS O_CREAT | O_WRONLY

#define SEM_INIT(name, value) (sem_open(name, SEM_FLAGS, 0666, value))
#define SEM_DESTROY(sem, name) \
    sem_close(sem);            \
    sem_unlink(name);

