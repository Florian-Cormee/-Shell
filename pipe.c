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

pipe_t *new_pipe()
{
    pipe_t *pipe = calloc(sizeof(cmd_t *), PIPE_SIZE);
    for (size_t i = 0; i < PIPE_SIZE; i++)
    {
        pipe[i] = new_cmd(NULL);
    }

    return pipe;
}

void delete_pipe(pipe_t **pipe)
{
    for (size_t i = 0; i < PIPE_SIZE; i++)
    {
        if (pipe[i] != NULL)
        {
            delete_cmd((*pipe) + i);
        }
    }
    free(*pipe);
    *pipe = NULL;
}

int parse_pipe(char *input, pipe_t *pipe)
{
    const char separator[] = {PIPE_OPERATOR, '\0'};
    char **tokArray = tokenize(input, separator);

    for (size_t i = 0; i < PIPE_SIZE; i++)
    {
        if (tokArray[i] == NULL)
        {
            return -1;
        }
        parse(tokArray[i], pipe[i]);
        free(tokArray[i]);
        tokArray[i] = NULL;
    }
    free(tokArray);
    return 0;
}

int close_pipe(int *pipefd)
{
    int returnVal = 0;
    if (close(pipefd[PIPE_IN]) == -1)
    {
        perror("[ERROR] Input pipe closing");
        returnVal = -1;
    }
    else if (close(pipefd[PIPE_OUT]) == -1)
    {
        perror("[ERROR] Output pipe closing");
        returnVal = -1;
    }
    return returnVal;
}

int execp(pipe_t *pipeCmd)
{
    int pipefd[2];
    pid_t pids[2];
    if (pipe(pipefd))
    {
        perror("[ERROR] Pipe creation");
    }

    for (size_t i = 0; i < PIPE_SIZE; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("[ERROR] Piped execution fork");
            return -1;
        }
        else if (pids[i] == 0)
        {
            dup2(pipefd[i == 0 ? 1 : 0], i == 0 ? STDOUT_FILENO : STDIN_FILENO);
            close_pipe(pipefd);
            run(pipeCmd[i]);
        }
    }

    close_pipe(pipefd);
    for (size_t i = 0; i < PIPE_SIZE; i++)
    {
        waitpid(pids[i], NULL, 0);
        debug("Piped process ended!");
    }

    return 0;
}

pipedCmd_t *new_pipedCmd()
{
    pipedCmd_t *pcmd = calloc(sizeof(pipedCmd_t), 1);
    pcmd->cmd = new_cmd(NULL);
    pcmd->output = calloc(sizeof(output_t), 1);
    pcmd->outputType = NONE;

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
        pcmd->cmd = NULL;
    }
    if (pcmd->output != NULL)
    {
        switch (pcmd->outputType)
        {
        case PATH:
            free(pcmd->output->outPath);
            break;
        case PIPED_CMD:
            delete_pipedCmd(&(pcmd->output->pcmd));
            break;
        default:
            break;
        }
        free(pcmd->output);
        pcmd->output = NULL;
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
            close_pipe(pipefd[i]);
            free(pipefd[i]);
            pipefd[i] = NULL;
        }
    }
}

int expipe(pipedCmd_t *pcmd)
{
    // Creates all requiered pipes; Maximum beeing 16
    size_t maxNbPipes = 16;
    int **pipefd = calloc(sizeof(int *), maxNbPipes);
    pid_t pids[16];
    size_t nbPipes = 0;
    pipedCmd_t *current = pcmd;
    while (current != NULL && current->outputType == PIPED_CMD)
    {
        if (nbPipes >= maxNbPipes)
        {
            break;
        }
        pipefd[nbPipes] = calloc(sizeof(int), 2);
        pipe(pipefd[nbPipes]);

        current = current->output->pcmd;
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
            if (current->outputType == PIPED_CMD && nbPids < nbPipes)
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
            if (current->outputType == PIPED_CMD)
            {
                current = current->output->pcmd;
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
