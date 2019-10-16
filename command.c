#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

cmd_t *create_cmd(char *name)
{
    cmd_t *cmd = (cmd_t *)malloc(sizeof(cmd_t));
    cmd->name = name;
    cmd->args = NULL;

    cmd->args = (char **)malloc(sizeof(char *));
    cmd->args[0] = NULL;
    return cmd;
}

void empty_cmd(cmd_t *cmd)
{
    if (cmd->name != NULL)
    {
        free(cmd->name);
        cmd->name = NULL;
    }
    if (cmd->args != NULL)
    {
        free(cmd->args);
        cmd->args = NULL;
    }
}

int exec(const cmd_t *cmd)
{
    if (cmd == NULL)
    {
        return -1;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    else if (pid == 0)
    {
        int r = execvp(cmd->name, cmd->args);
        if (r == -1)
        {
            perror("Execution error");
            exit(EXIT_FAILURE);
        }
        else
        {
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        int wstatus = EXIT_FAILURE;
        wait(&wstatus);
        return (WEXITSTATUS(wstatus) == EXIT_SUCCESS) ? 0 : -1;
    }
}
