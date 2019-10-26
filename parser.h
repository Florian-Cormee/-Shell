#ifndef PARSER_H
#define PARSER_H

#include "main.h"
#include "command.h"
#include "pipe.h"
#include <stdbool.h>

#define IS_PIPED(mod) (mod & PIPE_MOD)
#define IS_BACKGROUNDED(mod) (mod & BACKGROUND_MOD)
#define IS_PATH_OUT(mod) (mod & PATH_OUT_MOD)

int find(char **stringArray, const char*string);
/**
 * @brief Parses the input as an array of commands
 *
 * @param input Raw string input
 * @param cmds  The command
 * @return int 0 on success otherwise -1
 */
int parse(const char *input, cmd_t *cmds);

pipedCmd_t* pparse(const char* input);

bool is_background(const char*input);

char **tokenize(const char *s, const char *separator);

void delete_tokenArray(char ***s);

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
