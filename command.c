#include "command.h"
#include "boolean.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int exec(cmd_t *cmd)
{
    if (cmd == NULL)
    {
        return -1;
    }
    // Test for background
    size_t argc = length(cmd->args);
    bool background = argc > 1 && strcmp("&", cmd->args[argc - 1]) == 0;
    if (background)
    {
        cmd->args[argc - 1] = NULL;
    }
    // Executes the program in a new process
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
        if (background)
        {
            return 0;
        }
        else
        {
            int wstatus = EXIT_FAILURE;
            waitpid(pid, &wstatus, 0);
            return (WEXITSTATUS(wstatus) == EXIT_SUCCESS) ? 0 : -1;
        }
    }
}
