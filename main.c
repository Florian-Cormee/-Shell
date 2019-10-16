#include "main.h"
#include "command.h"
#include "parser.h"
#include "cd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    puts("Welcome in microShell");

    char input[INPUT_SIZE];
    input[0] = '\0';               // Initialize input as an empty string
    cmd_t *cmd = create_cmd(NULL); // Empty command initialization
    char wdir[WDIR_SIZE];

    while (strcmp(EXIT_CMD, input) != 0)
    {
        if(get_cwd(wdir, WDIR_SIZE) == NULL)
        {
            perror("[ERROR] Current working directory");
        }
        printf("%s$ ", wdir);

        if (!fgets(input, INPUT_SIZE, stdin))
        {
            perror("Getting input error");
            return -1;
        }
        str_replace(input, '\n', '\0');

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
            else if(strcmp(CD_CMD, cmd->name) == 0)
            {
                cwd(cmd);
            }
            else if(strcmp(PWD_CMD, cmd->name)==0)
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
            }
        }
        else
        {
            fprintf(stderr, "Parsing error\n");
        }
    }

    empty_cmd(cmd);
    free(cmd);

    return 0;
}
