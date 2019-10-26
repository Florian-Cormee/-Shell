#include "command.h"
#include "parser.h"
#include "logger.h"
#include "utils.h"
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

char *get_cwd(char *wdir, size_t size)
{
    getcwd(wdir, size);

    if (strcmp(getenv("HOME"), wdir) == 0)
    {
        strcpy(wdir, "~");
    }
    return wdir;
}

int change_dir(const char *input)
{
    char **argv = tokenize(input, ARG_SEPERATOR);
    puts("ici");
    for(size_t i = 0; argv[i] != NULL; i++)
    {
        debug(argv[i]);
    }
    char *path = argv[1];
    if (path == NULL)
    {
        // No path specified; Setting home directory
        path = getenv("HOME");
    }
    int rChDir = chdir(path);
    delete_tokenArray(&argv);
    return rChDir;
}
