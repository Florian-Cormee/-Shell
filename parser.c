#include "parser.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>

int parse(const char *input, cmd_t *cmd)
{
    clear_cmd(cmd);
    char **tokArray = tokenize(input, ARG_SEPERATOR);

    if (tokArray[0] == NULL)
    {
        free(tokArray);
        return -1;
    }
    cmd->name = strdup(tokArray[0]);
    cmd->args = tokArray;
    return 0;
}

char **tokenize(const char *s, const char *separator)
{
    char sCopy[INPUT_SIZE];
    strcpy(sCopy, s);
    char *token = strtok(sCopy, separator);

    if (token == NULL)
    {
        return NULL;
    }

    char **tokArray = calloc(sizeof(char *), MAX_TOKEN);

    for (size_t i = 0; token != NULL && i < MAX_TOKEN - 1; i++)
    {
        tokArray[i] = strdup(token);
        token = strtok(NULL, separator);
    }
    return tokArray;
}

void str_replace(char *s, char c, char replacement)
{
    for (size_t i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == c)
        {
            s[i] = replacement;
        }
    }
}

char get_modifiers(const char *input)
{
    char modifiers = 0;
    for (size_t i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == BACKGROUND_OPERATOR)
        {
            modifiers |= BACKGROUND_MOD;
        }
        else if (input[i] == PIPE_OPERATOR)
        {
            modifiers |= PIPE_MOD;
        }
    }
    return modifiers;
}
