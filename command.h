#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#define ARG_SEPERATOR " "

#define BACKGROUND_MODIFIER '&'
#define OUTPUT_MODIFIER '>'

/**
 * Structure representing a commande
 */
struct cmd {
    char *name;
    char **args;
    bool isBackground;
};

typedef struct cmd cmd_t;

cmd_t *new_cmd(char *name);

void clear_cmd(cmd_t *cmd);

void delete_cmd(cmd_t **cmd);

/**
 * Executes the given command in a seperate process
 * @return int Does not return on success. On failure return -1.
 */
int exec(cmd_t *cmd);

int run(cmd_t *cmd);

#endif
