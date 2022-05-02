#include "header.h"

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