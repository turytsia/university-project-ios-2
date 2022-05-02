#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <semaphore.h>

//error code
#define ERR_ARGS 10

//file constants
#define FILE_NAME "./proj2.out"
#define FILE_MODE "a"

//name constants
#define H_t 'H'
#define O_t 'O'

//message ids
#define ATOM_CREATED 20
#define ATOM_QUEUED 21
#define ATOM_DEQUEUED 22
#define ATOM_DONE 23
#define ATOM_LACK_H 24
#define ATOM_LACK_O 25

//creates new atom
#define ATOM_CREATE(id, type) ((atom_t){id, type})

//creates new process
#define PROCESS_CREATE(id, atom) ((process_t){id, atom, 0})

//returns random value to n
#define RANDOM_INTERVAL(n) (rand() % n)

//struct for args
typedef struct
{
    uint NH;
    uint NO;
    uint TI;
    uint TB;
    uint EXPECTED_H; 
    uint EXPECTED_O;
} args;

//struct for atom
typedef struct
{
    id_t id;
    char type;
} atom_t;

//struct for process
typedef struct
{
    pid_t id;
    atom_t atom;
    uint extra;
} process_t;

//struct for shared memory
typedef struct
{
    uint line_n;
    uint hydrogen_n;
    uint oxygen_n;
    uint molecule_counter_h;
    id_t molecule_id;
    key_t shmid;
} ipc_t;

//struct for semaphores
typedef struct
{
    sem_t *sem_hydrogen;
    sem_t *sem_oxygen; 
    sem_t*sem_output; 
    sem_t *sem_ready; 
    sem_t*sem_created; 
    sem_t*sem_creating;
    sem_t*sem_stop_extra;
} semaphores;

//function declarations

void err(int);

int read_args(args *, int, char **);

process_t fork_many(uint n, const char type);

void msg(const int action_id, process_t *process, ipc_t *ipc);

void sem_create(semaphores* sem);

void sem_cleanup(semaphores*sem);

ipc_t* ipc_init();

void ipc_destroy(ipc_t* ipc);
