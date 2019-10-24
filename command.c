#include "command.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

cmd_t *new_cmd(char *name)
{
    cmd_t *cmd = (cmd_t *)malloc(sizeof(cmd_t));
    cmd->name = name;
    cmd->isBackground = false;
    // cmd->outPath = NULL;

    cmd->args = (char **)malloc(sizeof(char *));
    cmd->args[0] = NULL;
    return cmd;
}

void clear_cmd(cmd_t *cmd)
{
    if (cmd->name != NULL)
    {
        free(cmd->name);
        cmd->name = NULL;
    }
    if (cmd->args != NULL)
    {
        for (size_t i = 0; cmd->args[i] != NULL; i++)
        {
            free(cmd->args[i]);
        }
        free(cmd->args);
        cmd->args = NULL;
    }
    cmd->isBackground = false;
    // if (cmd->outPath != NULL)
    // {
    //     free(cmd->outPath);
    //     cmd->outPath = NULL;
    // }
}

void delete_cmd(cmd_t **cmd)
{
    clear_cmd(*cmd);
    free(*cmd);
    *cmd = NULL;
}

int exec(cmd_t *cmd)
{
    if (cmd == NULL)
    {
        return -1;
    }
    // Test for background
    // size_t argc = length(cmd->args);
    // bool background = argc > 1 && strcmp("&", cmd->args[argc - 1]) == 0;
    // if (background)
    // {
    //     free(cmd->args[argc - 1]);
    //     cmd->args[argc - 1] = NULL;
    // }
    // Executes the program in a new process
    pid_t pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    else if (pid == 0)
    {
        return run(cmd);
    }
    else
    {
        if (cmd->isBackground)
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

int run(cmd_t *cmd)
{
    return execvp(cmd->name, cmd->args);
}
