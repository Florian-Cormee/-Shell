#ifndef PIPE_H
#define PIPE_H

#include "command.h"

#define PIPE_IN 0
#define PIPE_OUT 1

#define PIPE_SIZE 2
typedef cmd_t* pipe_t;
typedef struct pipedCmd pipedCmd_t;
union output
{
    char *outPath;
    pipedCmd_t *pcmd;
};
typedef union output output_t;

enum outputType {
    NONE, PIPED_CMD, PATH
};
typedef enum outputType outputType_t;

struct pipedCmd
{
    cmd_t *cmd;
    output_t* output;
    outputType_t outputType;
};


pipedCmd_t* new_pipedCmd();
void delete_pipedCmd(pipedCmd_t **cmd);

pipe_t * new_pipe();

void delete_pipe(pipe_t** pipe);

/**
 * @brief Parses a pipe from the input
 *
 * @param input The input from the user
 * @param pipe The address to store the parsed pipe
 * @return int 0 on success, otherwise -1
 */
int parse_pipe(char *input, pipe_t* pipe);

/**
 * @brief Executes piped commands
 *
 * @param cmd
 * @return int
 */
int execp(pipe_t* pipeCmd);

int execpcmd(pipedCmd_t *pcmd, bool waitForChild, int inputfd);

int expipe(pipedCmd_t *pcmd);

#endif // !PIPE_H
