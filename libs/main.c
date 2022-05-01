#include "header.h"

// prototypes
process_t fork_many(uint n, const char type);

void msg(const int action_id, process_t *process, ipc_t *ipc);

int main(int argc, char **argv)
{
    args arguments;

    process_t process;

    pid_t main_process_id = getpid();

    // reads arguments

    err(read_args(&arguments, argc, argv));

    // creates shared memory

    int ipc_key = IPC_ALLOCATE(IPC_KEY, ipc_t);

    ipc_t *ipc = IPC_AT(ipc_key);

    // creates semaphores
    sem_t *sem_hydrogen = SEM_INIT(SEMAPHORE_HYDROGEN, 0);

    sem_t *sem_oxygen = SEM_INIT(SEMAPHORE_OXYGEN, 1);

    sem_t *sem_output = SEM_INIT(SEMAPHORE_OUTPUT, 1);

    sem_t *sem_ready = SEM_INIT(SEMAPHORE_READY, 0);

    sem_t *sem_created = SEM_INIT(SEMAPHORE_CREATE, 0);

    sem_t *sem_creating = SEM_INIT(SEMAPHORE_CREATING, 0);

    sem_t *sem_stop_extra = SEM_INIT(SEMAPHORE_STOP_EXTRA, 0);

    if (arguments.EXPECTED_H == 0 && arguments.EXPECTED_O == 0)
    {
        sem_post(sem_stop_extra);
    }

    // creates NO processes from main
    if (main_process_id == getpid())
    {
        process = fork_many(arguments.NO, O_t);
    }

    // creates NH processes from main
    if (main_process_id == getpid())
    {
        process = fork_many(arguments.NH, H_t);
    }

    // NO + NH processes go here
    if (main_process_id != getpid())
    {
        srand(getpid());

        sem_wait(sem_output);

        msg(ATOM_CREATED, &process, ipc);

        sem_post(sem_output);

        if (arguments.TI > 0)
        {
            usleep(RANDOM_INTERVAL(arguments.TI));
        }

        sem_wait(sem_output);

        msg(ATOM_QUEUED, &process, ipc);

        sem_post(sem_output);

        switch (process.atom.type)
        {
        case H_t:
            ipc->hydrogen_n++;

            process.extra = arguments.EXPECTED_H < ipc->hydrogen_n;

            break;
        case O_t:
            ipc->oxygen_n++;

            process.extra = arguments.EXPECTED_O < ipc->oxygen_n;

            break;
        }

        if (process.extra == 1)
        {

            sem_wait(sem_stop_extra);

            msg(process.atom.type == H_t ? ATOM_LACK_H : ATOM_LACK_O, &process, ipc);

            sem_post(sem_stop_extra);

            exit(EXIT_SUCCESS);
        }

        if (process.atom.type == H_t)
        {
            sem_wait(sem_hydrogen);

            sem_wait(sem_output);
            msg(ATOM_DEQUEUED, &process, ipc);
            sem_post(sem_output);

            if (++ipc->molecule_counter_h >= 2)
            {
                sem_post(sem_creating);
                sem_post(sem_creating);
                sem_post(sem_creating);
            }

            sem_wait(sem_creating);

            if (arguments.TB > 0)
            {
                usleep(RANDOM_INTERVAL(arguments.TB));
            }

            sem_wait(sem_output);
            msg(ATOM_DONE, &process, ipc);
            sem_post(sem_output);

            --ipc->molecule_counter_h;

            sem_post(sem_ready);
        }
        else if (process.atom.type == O_t)
        {

            sem_wait(sem_oxygen);

            ++ipc->molecule_id;

            sem_wait(sem_output);
            msg(ATOM_DEQUEUED, &process, ipc);
            sem_post(sem_output);

            sem_post(sem_hydrogen);
            sem_post(sem_hydrogen);

            sem_wait(sem_creating);

            if (arguments.TB > 0)
            {
                usleep(RANDOM_INTERVAL(arguments.TB));
            }

            sem_wait(sem_output);
            msg(ATOM_DONE, &process, ipc);
            sem_post(sem_output);

            sem_wait(sem_ready);
            sem_wait(sem_ready);

            if (arguments.EXPECTED_O == ipc->molecule_id)
            {
                sem_post(sem_stop_extra);
            }

            sem_post(sem_oxygen);
        }
    }

    // waits for children to finish
    while (wait(NULL) > 0)
        ;

    // destroyes shared memory

    shmdt(ipc);

    shmctl(ipc_key, IPC_RMID, NULL);

    // destroyes semaphores

    SEM_DESTROY(sem_hydrogen, SEMAPHORE_HYDROGEN);

    SEM_DESTROY(sem_oxygen, SEMAPHORE_OXYGEN);

    SEM_DESTROY(sem_output, SEMAPHORE_OUTPUT);

    SEM_DESTROY(sem_ready, SEMAPHORE_READY);

    SEM_DESTROY(sem_created, SEMAPHORE_CREATE);

    SEM_DESTROY(sem_creating, SEMAPHORE_CREATING);

    SEM_DESTROY(sem_stop_extra, SEMAPHORE_STOP_EXTRA);

    return EXIT_SUCCESS;
}

// forks n processes of type t
process_t fork_many(uint n, const char type)
{
    for (id_t id = 1; id <= n; id++)
        if (fork() == 0)
            return PROCESS_CREATE(getpid(), ATOM_CREATE(id, type));

    return (process_t){0};
}

// message user about atom's action
void msg(const int action_id, process_t *process, ipc_t *ipc)
{
    FILE *output = fopen(FILE_NAME, FILE_MODE);

    ++ipc->line_n;

    switch (action_id)
    {
    case ATOM_CREATED:
        fprintf(output, "%d: %c %d: started\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    case ATOM_QUEUED:
        fprintf(output, "%d: %c %d: going to queue\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    case ATOM_DEQUEUED:
        fprintf(output, "%d: %c %d: creating molecule %d\n", ipc->line_n, process->atom.type, process->atom.id, ipc->molecule_id);
        break;
    case ATOM_DONE:
        fprintf(output, "%d: %c %d: molecule %d created\n", ipc->line_n, process->atom.type, process->atom.id, ipc->molecule_id);
        break;
    case ATOM_LACK_H:
        fprintf(output, "%d: %c %d: not enough O or H\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    case ATOM_LACK_O:
        fprintf(output, "%d: %c %d: not enough H\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    default:
        break;
    }

    fclose(output);
}