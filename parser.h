#ifndef PARSER_H
#define PARSER_H

#include "main.h"
#include "command.h"
#include <stdbool.h>

#define IS_PIPED(mod) (mod & PIPE_MOD)
#define IS_BACKGROUNDED(mod) (mod & BACKGROUND_MOD)

/**
 * @brief Parses the input as an array of commands
 *
 * @param input Raw string input
 * @param cmds  The command
 * @return int 0 on success otherwise -1
 */
int parse(const char *input, cmd_t *cmds);

char **tokenize(const char *s, const char *separator);

/**
 * @brief Replaces all occurences of c in s
 *
 * @param s The string to edit
 * @param c The char to replace
 * @param replacement The replacing char
 */
void str_replace(char *s, char c, char replacement);

char get_modifiers(const char *input);

#endif
