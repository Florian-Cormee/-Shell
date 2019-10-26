#include "main.h"
#include "cd.h"
#include "command.h"
#include "logger.h"
#include "parser.h"
#include "pipe.h"
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
    set_level(DEBUG);
    puts("Welcome in microShell");

    char input[INPUT_SIZE];
    // char modifiers = 0;
    memset(input, '\0', INPUT_SIZE); // Initialize input as an empty string
    // cmd_t *cmd = new_cmd(NULL);      // Empty command initialization
    print_cwd();

    while (true)
    {
        // Gets the input from stdin
        if (!fgets(input, INPUT_SIZE, stdin))
        {
            perror("Getting input error");
            return -1;
        }
        str_replace(input, '\n', '\0');

        printf("Input : \"%s\"\n", input);

        if (strncmp(EXIT_CMD, input,strlen(EXIT_CMD)) == 0)
        {
            break;
        }
        else if (strncmp(CD_CMD, input, strlen(CD_CMD)) == 0)
        {
            change_dir(input);
        }
        else if (strncmp(PWD_CMD, input, strlen(PWD_CMD)) == 0)
        {
            puts(wdir);
        }
        else
        {
            // Parses the command out of the input
            pipedCmd_t *pcmd = pparse(input);
            expipe(pcmd, !is_background(input));
            delete_pipedCmd(&pcmd);
        }
        /*modifiers = get_modifiers(input);

        if (IS_PIPED(modifiers))
        {
            puts("[DEBUG] Piped");
            pipe_t *pipeCmds = new_pipe();
            if (parse_pipe(input, pipeCmds) == 0)
            {
                if (execp(pipeCmds) == 0)
                {
                    puts("Pipe done!");
                }
                else
                {
                    perror("[ERROR] Piped execution");
                }
            }
            delete_pipe(&pipeCmds);
        }
        else
        {
            if (parse(input, cmd) != -1)
            {
                debug("Parsed input:");
                printf("Name : %s\nArguments :\n", cmd->name);

                for (int i = 0; cmd->args[i] != NULL; i++)
                    puts(cmd->args[i]);
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
        }*/

        print_cwd();
    }

    // clear_cmd(cmd);
    // free(cmd);

    return 0;
}
