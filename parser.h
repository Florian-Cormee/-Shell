#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "command.h"
#include "main.h"

#define IS_PIPED(mod) (mod & PIPE_MOD)
#define IS_BACKGROUNDED(mod) (mod & BACKGROUND_MOD)
#define IS_PATH_OUT(mod) (mod & PATH_OUT_MOD)

/**
 * @brief Replaces all occurences of c in s
 *
 * @param s The string to edit
 * @param c The char to replace
 * @param replacement The replacing char
 */
void str_replace(char *s, char c, char replacement);

/**
 * @brief Breaks down s at each separator into an array of token
 *
 * Logs error message on failure
 *
 * @param s The string to tokenize
 * @param separator The delimiter of a token
 * @return char** On success, the array of tokens; otherwise NULL
 */
char **tokenize(const char *s, const char *separator);

/**
 * @brief Frees the array of token and every tokens it has
 *
 * @param p_token_array The pointer to the array of token to free
 */
void delete_token_array(char ***p_token_array);

/**
 * @brief Finds the index of the given string in the array
 *
 * Silent on failure
 *
 * @param string_array The array to look up; must end by NULL
 * @param string The string to look for
 * @return The index otherwise -1
 */
int find(char **string_array, const char *string);

/**
 * @brief Parses a whole command from the input
 *
 * Parses the first command and its child commands for piping
 *
 * Logs error message on failure
 *
 * @param input The input from the user
 * @param is_background True if the command shoud be run in background;
 * otherwise false
 * @return command_t* The parsed command; on failure NULL
 */
command_t *parse_cmd(const char *input, bool *is_background);

#endif
