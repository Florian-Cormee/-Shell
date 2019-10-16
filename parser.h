#ifndef PARSER_H
#define PARSER_H

#include "command.h"

#define MAX_TOKEN 32
#define ARG_SEPERATOR " "

/**
 * @brief Parses the input as an array of commands
 *
 * @param input Raw string input
 * @param cmds  The command
 * @return int 0 on success otherwise -1
 */
int parse(char *input, cmd_t *cmds);

char **tokenize(char *s, const char *separator);

/**
 * @brief Replaces all occurences of c in s
 *
 * @param s The string to edit
 * @param c The char to replace
 * @param replacement The replacing char
 */
void str_replace(char *s, char c, char replacement);
#endif
