#include "pipe.h"
#include "command.h"
#include "logger.h"
#include "main.h"
#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pipedCmd_t *new_pipedCmd()
{
    pipedCmd_t *pcmd = calloc(sizeof(pipedCmd_t), 1);
    pcmd->cmd = new_cmd(NULL);
    pcmd->outPath = NULL;
    pcmd->childCmd = NULL;
    return pcmd;
}

void delete_pipedCmd(pipedCmd_t **ppcmd)
{
    if (ppcmd == NULL || *ppcmd == NULL)
    {
        return;
    }

    pipedCmd_t *pcmd = *ppcmd;
    if (pcmd->cmd != NULL)
    {
        delete_cmd(&(pcmd->cmd));
    }
    if (pcmd->childCmd != NULL)
    {
        delete_pipedCmd(&(pcmd->childCmd));
    }
    if (pcmd->outPath != NULL)
    {
        free(pcmd->outPath);
    }
    free(pcmd);
    *ppcmd = NULL;
}

void close_all_pipes(int **pipefd, size_t amount)
{
    for (size_t i = 0; i < amount; i++)
    {
        if (pipefd[i] != NULL)
        {
            close(pipefd[i][PIPE_IN]);
            close(pipefd[i][PIPE_OUT]);
            free(pipefd[i]);
            pipefd[i] = NULL;
        }
    }
}

int expipe(pipedCmd_t *pcmd, bool waitChildren)
{
    // Creates all requiered pipes; Maximum beeing 16
    size_t maxNbPipes = 16;
    int **pipefd = calloc(sizeof(int *), maxNbPipes);
    pid_t pids[16];
    size_t nbPipes = 0;
    pipedCmd_t *current = pcmd;
    while (current != NULL && current->childCmd != NULL)
    {
        if (nbPipes >= maxNbPipes)
        {
            break;
        }
        pipefd[nbPipes] = calloc(sizeof(int), 2);
        pipe(pipefd[nbPipes]);

        current = current->childCmd;
        nbPipes++;
    }

    // Creates child processes
    current = pcmd;
    size_t nbPids = 0;
    while (current != NULL && nbPids < 16)
    {
        printf("Current: %p\n", current);
        pids[nbPids] = fork(); // TODO handle failure
        if (pids[nbPids] == 0)
        {
            printf("%s\n", current->cmd->name);
            if (nbPids > 0 && nbPids - 1 < nbPipes)
            {
                printf("%s : redirection de l'entrée\n", current->cmd->name);
                dup2(pipefd[nbPids - 1][PIPE_IN], STDIN_FILENO);
            }
            if (current->outPath != NULL)
            {
                FILE *f = fopen(current->outPath, "w");
                int fd = fileno(f);
                dup2(fd, STDOUT_FILENO);
                fclose(f);
            }
            else if (current->childCmd != NULL && nbPids < nbPipes)
            {
                printf("%s : redirection de la sortie\n", current->cmd->name);
                dup2(pipefd[nbPids][PIPE_OUT], STDOUT_FILENO);
            }
            close_all_pipes(pipefd, maxNbPipes);
            run(current->cmd);
            exit(EXIT_FAILURE);
            return -1;
        }
        else
        {
            printf("Pid : %d\n", pids[nbPids]);
            if (current->childCmd != NULL)
            {
                current = current->childCmd;
            }
            else
            {
                current = NULL;
            }
        }
        nbPids++;
    }
    close_all_pipes(pipefd, maxNbPipes);
    free(pipefd);

    if (waitChildren)
    {
        // Wait for every children
        for (size_t i = 0; i < nbPids; i++)
        {
            printf("Waiting for child %ld / %ld\n", i + 1, nbPids);
            int processExitValue = -1;
            int waitSuccess = waitpid(pids[i], &processExitValue, 0);
            printf("Waiting done : %d has exited %d, has signaled: %d, exit "
                   "status: %d, signal: %d\n",
                   waitSuccess, WIFEXITED(processExitValue),
                   WTERMSIG(processExitValue), WEXITSTATUS(processExitValue),
                   WTERMSIG(processExitValue));
            perror("Wait");
        }
        return 0;
    }
}
