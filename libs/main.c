#include "header.h"

// prototypes
process_t fork_many(uint n, const char type);

void msg(const int action_id, process_t *process, ipc_t *ipc);

int main(int argc, char **argv)
{
    args arguments;

    process_t process;

    semaphores sem;

    ipc_t *ipc = NULL;

    pid_t main_process_id = getpid();

    // reads arguments

    err(read_args(&arguments, argc, argv));

    // creates shared memory

    ipc = ipc_init();

    // creates semaphores

    sem_create(&sem);

    if (arguments.EXPECTED_H == 0 && arguments.EXPECTED_O == 0)
    {
        sem_post(sem.sem_stop_extra);
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

        sem_wait(sem.sem_output);

        msg(ATOM_CREATED, &process, ipc);

        sem_post(sem.sem_output);

        if (arguments.TI > 0)
        {
            usleep(RANDOM_INTERVAL(arguments.TI));
        }

        sem_wait(sem.sem_output);

        msg(ATOM_QUEUED, &process, ipc);

        sem_post(sem.sem_output);

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

            sem_wait(sem.sem_stop_extra);

            msg(process.atom.type == H_t ? ATOM_LACK_H : ATOM_LACK_O, &process, ipc);

            sem_post(sem.sem_stop_extra);

            exit(EXIT_SUCCESS);
        }

        if (process.atom.type == H_t)
        {
            sem_wait(sem.sem_hydrogen);

            sem_wait(sem.sem_output);
            msg(ATOM_DEQUEUED, &process, ipc);
            sem_post(sem.sem_output);

            if (++ipc->molecule_counter_h >= 2)
            {
                sem_post(sem.sem_creating);
                sem_post(sem.sem_creating);
                sem_post(sem.sem_creating);
            }

            sem_wait(sem.sem_creating);

            if (arguments.TB > 0)
            {
                usleep(RANDOM_INTERVAL(arguments.TB));
            }

            sem_wait(sem.sem_output);
            msg(ATOM_DONE, &process, ipc);
            sem_post(sem.sem_output);

            --ipc->molecule_counter_h;

            sem_post(sem.sem_ready);

            exit(EXIT_SUCCESS);
        }
        else if (process.atom.type == O_t)
        {

            sem_wait(sem.sem_oxygen);

            ++ipc->molecule_id;

            sem_wait(sem.sem_output);
            msg(ATOM_DEQUEUED, &process, ipc);
            sem_post(sem.sem_output);

            sem_post(sem.sem_hydrogen);
            sem_post(sem.sem_hydrogen);

            sem_wait(sem.sem_creating);

            if (arguments.TB > 0)
            {
                usleep(RANDOM_INTERVAL(arguments.TB));
            }

            sem_wait(sem.sem_output);
            msg(ATOM_DONE, &process, ipc);
            sem_post(sem.sem_output);

            sem_wait(sem.sem_ready);
            sem_wait(sem.sem_ready);

            if (arguments.EXPECTED_O == ipc->molecule_id)
            {
                sem_post(sem.sem_stop_extra);
            }

            sem_post(sem.sem_oxygen);

            exit(EXIT_SUCCESS);
        }
    }

    // waits for children to finish
    while (wait(NULL) > 0)
        ;


    //cleanup

    sem_cleanup(&sem);

    ipc_destroy(ipc);

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
        fprintf(output, "%u: %c %u: started\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    case ATOM_QUEUED:
        fprintf(output, "%u: %c %u: going to queue\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    case ATOM_DEQUEUED:
        fprintf(output, "%u: %c %u: creating molecule %u\n", ipc->line_n, process->atom.type, process->atom.id, ipc->molecule_id);
        break;
    case ATOM_DONE:
        fprintf(output, "%u: %c %u: molecule %u created\n", ipc->line_n, process->atom.type, process->atom.id, ipc->molecule_id);
        break;
    case ATOM_LACK_H:
        fprintf(output, "%u: %c %u: not enough O or H\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    case ATOM_LACK_O:
        fprintf(output, "%u: %c %u: not enough H\n", ipc->line_n, process->atom.type, process->atom.id);
        break;
    default:
        break;
    }

    fclose(output);
}