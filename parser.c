#include "parser.h"
#include "logger.h"
#include "main.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find(char **stringArray, const char *string)
{
    for (int i = 0; stringArray[i] != NULL; i++)
    {
        if (strcmp(stringArray[i], string) == 0)
        {
            return i;
        }
    }
    return -1;
}

int parse(const char *input, cmd_t *cmd)
{
    clear_cmd(cmd);
    char **tokArray = tokenize(input, ARG_SEPERATOR);

    if (tokArray[0] == NULL)
    {
        delete_tokenArray(&tokArray);
        return -1;
    }

    char modifiers = get_modifiers(input);
    if (IS_BACKGROUNDED(modifiers))
    {
        cmd->isBackground = true;
        const char operator[] = {BACKGROUND_OPERATOR, '\0'};
        size_t index = find(tokArray, operator);
        if (index != -1)
        {
            free(tokArray[index]);
            tokArray[index] = NULL;
        }
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

void delete_tokenArray(char ***s)
{
    char **tokArray = *s;
    if (tokArray != NULL)
    {
        for (size_t i = 0; tokArray[i] != NULL; i++)
        {
            free(tokArray[i]);
        }
        free(tokArray);
        *s = NULL;
    }
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

bool is_background(const char *input)
{
    size_t length = strlen(input);
    for (size_t i = 0; i < length; i++)
    {
        size_t index = length - 1 - i;
        if (input[index] == ' ')
        {
            continue;
        }
        return input[index] == BACKGROUND_OPERATOR;
    }
    return false;
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

pipedCmd_t *pparse(const char *input)
{
    // Piped command separator as a string
    const char pipeSeparator[] = {PIPE_OPERATOR, '\0'};
    // Standard output redirection operator as a string
    const char outputOperator[] = {OUTPUT_MODIFIER, '\0'};
    // Interrups parsing tasks and jumps to freeing memory
    bool hasError = false;
    // Input tokenized as piped command
    char **pipedInput = tokenize(input, pipeSeparator);
    size_t pipedInputCount = length(pipedInput);
    // Command array
    pipedCmd_t **inputArray = calloc(sizeof(pipedCmd_t *), pipedInputCount);
    // Command's arguments
    char **args;
    size_t argsLength;

    for (size_t i = 0; i < pipedInputCount; i++)
    {
        if (is_background(pipedInput[i]))
        {
            error("Cannot run in background a piped command!");
            hasError = true;
            break;
        }
        // Parses each command without linking piped output
        inputArray[i] = new_pipedCmd();
        int result = parse(pipedInput[i], inputArray[i]->cmd);
        if (result == -1)
        {
            // On failed parsing exit function
            hasError = true;
            break;
        }

        // File output parsing
        args = inputArray[i]->cmd->args;
        argsLength = length(args);
        if (argsLength >= 2 &&
            strcmp(args[argsLength - 2], outputOperator) == 0)
        {
            inputArray[i]->outPath = args[argsLength - 1];
            // Remove modifier and value from the list of arguments
            free(args[argsLength - 2]);
            args[argsLength - 2] = NULL;
            args[argsLength - 1] = NULL;
        }
    }
    // Links the piped output
    for (size_t i = 1; i < pipedInputCount && !hasError; i++)
    {
        pipedCmd_t *parent = inputArray[pipedInputCount - i - 1];
        printf("Parent : %s\n", parent->cmd->name);
        pipedCmd_t *child = inputArray[pipedInputCount - i];
        if (child != NULL)
        {
            printf("Child : %s\n", child->cmd->name);
        }
        else
        {
            puts("Childless");
        }
        parent->childCmd = child;
    }

    pipedCmd_t *pipedCmd = NULL;
    delete_tokenArray(&pipedInput);
    if (hasError)
    {
        for (size_t i = 0; inputArray[i] != NULL; i++)
        {
            delete_pipedCmd(&(inputArray[i]));
        }
    }
    else
    {
        pipedCmd = inputArray[0];
    }
    free(inputArray);
    return pipedCmd;
}
