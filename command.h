#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

#define PIPE_IN 0
#define PIPE_OUT 1

typedef struct command command_t;

struct command {
    char *file;
    char **args;
    char *output_path;
    command_t *child;
};

/**
 * @brief Creates an empty command
 *
 * @return command_t* The pointer to the created command
 */
command_t *new_command();

/**
 * @brief Frees the command and ALL its fields
 *
 * @param p_cmd The pointer to the command to free
 */
void delete_command(command_t **p_cmd);

/**
 * @brief Set the file
 *
 * Frees the old value
 * Duplicates the file
 * Sets the copy in cmd
 *
 * @param cmd The command
 * @param file The file to duplicate and set
 */
void set_file(command_t *cmd, const char *file);

/**
 * @brief Set the args
 *
 * Frees the old value
 * Sets args in cmd
 *
 * NOTE: Does not duplicate args
 *
 * @param cmd The command
 * @param args The arguments
 */
void set_args(command_t *cmd, char **args);

/**
 * @brief Set the output of the command
 *
 * Frees the old value
 * Duplicates the path
 * Sets the copy in cmd
 *
 * @param cmd The command
 * @param path The output path to duplicate and set
 */
void set_output(command_t *cmd, const char *path);

/**
 * @brief Set the child command
 *
 * Frees the old value
 * Set child in cmd
 *
 * NOTE: Does not duplicate child
 *
 * @param cmd The command
 * @param child The command's child (aka. pipe receiver)
 */
void set_child(command_t *cmd, command_t *child);

/**
 * @brief Executes the command
 *
 * Creates child processes and pipes to execute the given command
 *
 * @param cmd The command to execute
 * @param waitChildren If TRUE the main process will wait for child processes
 * @return int O on success otherwise -1
 */
int execute(command_t *cmd, bool waitChildren);

#endif
