#ifndef PIPE_H
#define PIPE_H

#include "command.h"

#define PIPE_IN 0
#define PIPE_OUT 1

#define PIPE_SIZE 2
typedef cmd_t* pipe_t;

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

#endif // !PIPE_H
