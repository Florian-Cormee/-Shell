#include "parser.h"
#include <stdlib.h>
#include <string.h>

int parse(char *input, cmd_t *cmd)
{
    empty_cmd(cmd);
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

char **tokenize(char *s, const char *separator)
{
    size_t i = 0;
    char **tokArray = calloc(sizeof(char *), MAX_TOKEN);
    char *token = strtok(s, separator);
    tokArray[0] = token;

    if (tokArray[0] == NULL)
    {
        return tokArray;
    }

    do
    {
        i++;
        token = strtok(NULL, separator);
        tokArray[i] = token;
    } while (token != NULL);
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
