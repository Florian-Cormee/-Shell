#include "main.h"
#include "cd.h"
#include "command.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char wdir[WDIR_SIZE];

void print_cwd()
{
    if (get_cwd(wdir, WDIR_SIZE) == NULL)
    {
        perror("[ERROR] Current working directory");
    }
    printf("%s$ ", wdir);
}
int main(void)
{
    puts("Welcome in microShell");

    char input[INPUT_SIZE];
    memset(input, '\0', INPUT_SIZE); // Initialize input as an empty string
    cmd_t *cmd = create_cmd(NULL);   // Empty command initialization

    print_cwd();

    while (strcmp(EXIT_CMD, input) != 0)
    {
        // Gets the input from stdin
        if (!fgets(input, INPUT_SIZE, stdin))
        {
            perror("Getting input error");
            return -1;
        }
        str_replace(input, '\n', '\0');
        // Parses the command out of the input
        if (parse(input, cmd) != -1)
        {
#ifdef DEBUG
            printf("Name : %s\nArguments :\n", cmd->name);
            for (int i = 0; cmd->args[i] != NULL; i++)
                puts(cmd->args[i]);
#endif // DEBUG
            if (strcmp(EXIT_CMD, cmd->name) == 0)
            {
                break;
            }
            else if (strcmp(CD_CMD, cmd->name) == 0)
            {
                cwd(cmd);
            }
            else if (strcmp(PWD_CMD, cmd->name) == 0)
            {
                puts(wdir);
            }
            else
            {
                int r = exec(cmd);
                if (r == -1)
                {
                    fprintf(stderr, "Exec exception\n");
                }
                else
                {
                    puts("Done");
                }
            }
        }
        else
        {
            fprintf(stderr, "Parsing error\n");
        }

        print_cwd();
    }

    empty_cmd(cmd);
    free(cmd);

    return 0;
}
