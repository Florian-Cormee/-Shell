#ifndef CD_H
#define CD_H
#include "command.h"
#include <stddef.h>

void cwd(cmd_t *cmd);
int change_dir(const char *input);
char *get_cwd(char *wdir, size_t size);
#endif
