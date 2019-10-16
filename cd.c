#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cwd(cmd_t *cmd)
{
    char *path = cmd->args[1];
    if (path == NULL)
    {
        path = getenv("HOME");
    }
    if (chdir(path) == -1)
    {
        perror("[ERROR] cd");
    }
}

char* get_cwd(char *wdir, size_t size)
{
    getcwd(wdir, size);

    if (strcmp(getenv("HOME"), wdir) == 0)
    {
        strcpy(wdir, "~");
    }
    return wdir;
}
