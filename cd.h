#ifndef CD_H
#define CD_H
#include "command.h"
#include <stdlib.h>

void cwd(cmd_t *cmd);
char *get_cwd(char *wdir, size_t size);
#endif
