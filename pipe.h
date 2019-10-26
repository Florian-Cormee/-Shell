#ifndef PIPE_H
#define PIPE_H

#include "command.h"

#define PIPE_IN 0
#define PIPE_OUT 1

#define PIPE_SIZE 2
typedef struct pipedCmd pipedCmd_t;

struct pipedCmd
{
    cmd_t *cmd;
    char *outPath;
    pipedCmd_t *childCmd;
};

pipedCmd_t *new_pipedCmd();

void delete_pipedCmd(pipedCmd_t **cmd);

int expipe(pipedCmd_t *pcmd, bool waitChildren);

#endif // !PIPE_H
