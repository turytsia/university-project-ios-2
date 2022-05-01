#include "header.h"

#define ARGS_COUNT 5

#define is_num(n) (n >= '0' && n <= '9')

int read_args(args *arguments, int argc, char **argv)
{

    if (argc != ARGS_COUNT)
    {
        return ERR_ARGS;
    }

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '\0')
        {
            return ERR_ARGS;
        }
        // check if args is a number
        for (int j = 0; argv[i][j]; j++)
            if (!is_num(argv[i][j]))
                return ERR_ARGS;

        uint arg_value = (unsigned)atoi(argv[i]);

        

        switch (i)
        {
        case 1:
            if (arg_value < 1)
            {
                return EXIT_FAILURE;
            }
            arguments->NO = arg_value;
            break;
        case 2:
            if (arg_value < 1)
            {
                return EXIT_FAILURE;
            }
            arguments->NH = arg_value;
            break;
        case 3:
            if (arg_value > 1000)
            {
                return EXIT_FAILURE;
            }
            arguments->TI = arg_value * 1000;
            break;
        case 4:
            if (arg_value > 1000)
            {
                return EXIT_FAILURE;
            }
            arguments->TB = arg_value * 1000;
            break;
        }
    }

    arguments->EXPECTED_H = arguments->NH;

    arguments->EXPECTED_O = arguments->NO;

    if (arguments->NH % 2 == 1)
    {
        arguments->EXPECTED_H--;
    }

    if (arguments->NH >= arguments->NO * 2)
    {
        arguments->EXPECTED_H = arguments->EXPECTED_O * 2;
    }
    else
    {
        arguments->EXPECTED_O = arguments->EXPECTED_H / 2;
    }

    return EXIT_SUCCESS;
}