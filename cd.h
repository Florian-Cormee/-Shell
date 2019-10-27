#ifndef CD_H
#define CD_H

#include <stddef.h>
#include "command.h"

/**
 * @brief Get the current working directory
 *
 * @param wdir An existing array of char
 * @param size The size of this array
 * @return On success, wdir; otherwise NULL
 */
char *get_cwd(char *wdir, size_t size);

/**
 * @brief Set the current working directory from the input
 *
 * Logs an error on failure
 * 
 * @param input The user input
 * @return 0 on success; otherwise -1
 */
int set_cwd(const char *input);
#endif
