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
    if(close(pipefd[PIPE_IN]) == -1)
    {
        perror("[ERROR] Input pipe closing");
        returnVal = -1;
    }
    else if(close(pipefd[PIPE_OUT]) == -1)
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
