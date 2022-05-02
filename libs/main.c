#include "header.h"

/*
 *   xturyt00
 */

int main(int argc, char **argv)
{
    /*
     *   Removes a file after the first launch
     */
    remove(FILE_NAME);
    /*
     *   Variables
     */
    args arguments;

    process_t process;

    semaphores sem;

    ipc_t *ipc = NULL;

    pid_t main_process_id = getpid();
    /*
     *   Reads arguments
     */
    err(read_args(&arguments, argc, argv));
    /*
     *   Creates shared memory space
     */
    ipc = ipc_init();
    /*
     *   Creates semaphores
     */
    sem_create(&sem);
    /*
     *   In case if there is not enough atoms to be created
     */
    if (arguments.EXPECTED_H == 0 && arguments.EXPECTED_O == 0)
    {
        sem_post(sem.sem_stop_extra);
    }
    /*
     *   Creates NO processes out of main process
     */
    if (main_process_id == getpid())
    {
        process = fork_many(arguments.NO, O_t);
    }
    /*
     *   Creates NH processes out of main process
     */
    if (main_process_id == getpid())
    {
        process = fork_many(arguments.NH, H_t);
    }

    if (main_process_id != getpid())
    {
        /*
         *   Srand init, for random calls
         */
        srand(getpid());
        /*
         *   Writes in a file, that atom was created
         */
        sem_wait(sem.sem_output);

        msg(ATOM_CREATED, &process, ipc);

        sem_post(sem.sem_output);
        /*
         *   After atom was created, waits random time to TI to enqueue
         */
        usleep(arguments.TI > 0 ? RANDOM_INTERVAL(arguments.TI) * 1000 : 0);
        /*
         *   Writes in a file that atom was queued
         */
        sem_wait(sem.sem_output);

        msg(ATOM_QUEUED, &process, ipc);

        sem_post(sem.sem_output);
        /*
         *   Counts extra atoms
         */
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
        /*
         *   if atom is extra, then release them in the end
         */
        if (process.extra == 1)
        {

            sem_wait(sem.sem_stop_extra);

            msg(process.atom.type == H_t ? ATOM_LACK_H : ATOM_LACK_O, &process, ipc);

            sem_post(sem.sem_stop_extra);

            exit(EXIT_SUCCESS);
        }
        /*
         *   if atom is not extra
         */
        if (process.atom.type == H_t)
        {
            /*
             *   Here creates queue of H atoms
             */
            sem_wait(sem.sem_hydrogen);
            /*
             *   Writes in a file that atom was dequeued
             */
            sem_wait(sem.sem_output);

            msg(ATOM_DEQUEUED, &process, ipc);

            sem_post(sem.sem_output);
            /*
             *   Making sure that here are 2 processes
             */
            if (++ipc->molecule_counter_h >= 2)
            {
                sem_post(sem.sem_creating);

                sem_post(sem.sem_creating);

                sem_post(sem.sem_creating);
            }
            /*
             *   If theres less than 2 processes then wait for the second one
             */
            sem_wait(sem.sem_creating);

            sem_wait(sem.sem_output);
            /*
             *   Writes in a file that molecule is created
             */
            msg(ATOM_DONE, &process, ipc);

            sem_post(sem.sem_output);
            /*
             *   Decrements flag to validate 2 hydrogen atoms here to accept next ones correctly
             */
            --ipc->molecule_counter_h;
            /*
             *   Sends signal to oxygen
             */
            sem_post(sem.sem_ready);

            exit(EXIT_SUCCESS);
        }
        else if (process.atom.type == O_t)
        {
            /*
             *   Creates a queue of oxygen atoms
             */
            sem_wait(sem.sem_oxygen);
            /*
             *   Calculates molecule id
             */
            ++ipc->molecule_id;
            /*
             *   Writes in a file that oxygen was dequeued
             */
            sem_wait(sem.sem_output);

            msg(ATOM_DEQUEUED, &process, ipc);

            sem_post(sem.sem_output);
            /*
             *   Unblocks 2 atoms of hydrogen
             */
            sem_post(sem.sem_hydrogen);

            sem_post(sem.sem_hydrogen);
            /*
             *   Waits random time to build up a molecule
             */
            usleep(arguments.TB > 0 ? RANDOM_INTERVAL(arguments.TB) * 1000 : 0);
            /*
             *   Syncs with hydrogen
             */
            sem_wait(sem.sem_creating);
            /*
             *   Messages that molecule is created
             */
            sem_wait(sem.sem_output);

            msg(ATOM_DONE, &process, ipc);

            sem_post(sem.sem_output);
            /*
             *   Waits for hydrogen to finish
             */
            sem_wait(sem.sem_ready);

            sem_wait(sem.sem_ready);
            /*
             *   In case if this is the last - release the others
             */
            if (arguments.EXPECTED_O == ipc->molecule_id)
            {
                sem_post(sem.sem_stop_extra);
            }
            /*
             *   Ublock the next oxygen
             */
            sem_post(sem.sem_oxygen);

            exit(EXIT_SUCCESS);
        }
    }

    /*
     *   Parent waits for the children to finish the process
     */
    while (wait(NULL) > 0)
        ;

    /*
     *   Cleanup functions
     */

    sem_cleanup(&sem);

    ipc_destroy(ipc);

    return EXIT_SUCCESS;
}