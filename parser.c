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
    bool hasError = false;
    char modifiers = get_modifiers(input);
    if (IS_PIPED(modifiers) && IS_BACKGROUNDED(modifiers))
    {
        // Incompatible operators
        error("Incompatible operators found: & |");
        return NULL;
    }
    const char pipeSeparator[] = {PIPE_OPERATOR, '\0'};
    char **pipedInput = tokenize(input, pipeSeparator);
    size_t pipedInputCount = length(pipedInput);
    pipedCmd_t **inputArray = calloc(sizeof(pipedCmd_t *), pipedInputCount);

    for (size_t i = 0; i < pipedInputCount; i++)
    {
        // Parses each command without linking piped output
        inputArray[i] = new_pipedCmd();
        int result = parse(pipedInput[i], inputArray[i]->cmd);
        if (result == -1)
        {
            // On failed parsing exit function
            hasError = true;
            break;
        }
        printf("Parsed : %s\n", inputArray[i]->cmd->name);
        // File output parsing
        const char outputOperator[] = {OUTPUT_MODIFIER, '\0'};
        cmd_t *cmd = inputArray[i]->cmd;
        char **args = cmd->args;
        size_t argsLength = length(args);
        if (argsLength >= 2 &&
            strcmp(args[argsLength - 2], outputOperator) == 0)
        {
            inputArray[i]->outputType = PATH;
            inputArray[i]->output->outPath = args[argsLength - 1];
            // Remove modifier and value from the list of arguments
            free(args[argsLength - 2]);
            args[argsLength - 2] = NULL;
            args[argsLength - 1] = NULL;
        }
    }
    // Links the piped output
    for (size_t i = 1; i < pipedInputCount; i++)
    {
        pipedCmd_t *parent = inputArray[pipedInputCount - i - 1];
        printf("Parent : %s\n", parent->cmd->name);
        if (parent->outputType == NONE)
        {
            parent->outputType = PIPED_CMD;
            pipedCmd_t *child = inputArray[pipedInputCount - i];
            if (child != NULL)
            {
                printf("Child : %s\n", child->cmd->name);
            }
            else
            {
                puts("Childless");
            }
            parent->output->pcmd = child;
        }
        else
        {
            warn("Conflicting output; cancelling pipes");
            // On already defined output, delete all previous children
            delete_pipedCmd(&(inputArray[pipedInputCount - i]));
        }
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
