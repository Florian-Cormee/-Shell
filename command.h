#ifndef COMMAND_H
#define COMMAND_H

/**
 * Structure representing a commande
 */
struct cmd {
    char *name;
    char **args;
};

typedef struct cmd cmd_t;

cmd_t *create_cmd(char *name);

void empty_cmd(cmd_t *cmd);

/**
 * Executes the given command in a seperate process
 * @return int Does not return on success. On failure return -1.
 */
int exec(cmd_t *cmd);

#endif
