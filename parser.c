#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "main.h"
#include "utils.h"

void str_replace(char *s, char c, char replacement) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == c) {
            s[i] = replacement;
        }
    }
}

char **tokenize(const char *s, const char *separator) {
    // Copies the argument s
    char sCopy[INPUT_SIZE];
    strcpy(sCopy, s);

    // Tokenizes the copy
    char *token = strtok(sCopy, separator);
    if (token == NULL) {
        log_f(WARN, "No token has been found in '%s'.", s);
        return NULL;
    }

    char **tokArray = calloc(sizeof(char *), MAX_TOKEN);
    if (tokArray == NULL) {
        log_err(ERROR, "Memory allocation for an array of token has failed");
        return NULL;
    }

    for (size_t i = 0; token != NULL && i < MAX_TOKEN - 1; i++) {
        tokArray[i] = strdup(token); // Saves the latest token
        if (tokArray[i] == NULL) {
            log_err(ERROR, "String duplication has failed");
            delete_token_array(&tokArray);
            return NULL;
        }

        token = strtok(NULL, separator); // Retrieves the next token
    }
    return tokArray;
}

void delete_token_array(char ***s) {
    if (s == NULL) return;

    char **tokArray = *s;
    if (tokArray != NULL) {
        for (size_t i = 0; tokArray[i] != NULL; i++) {
            free(tokArray[i]);
        }
        free(tokArray);
        *s = NULL;
    }
}

int find(char **stringArray, const char *string) {
    if (stringArray == NULL || string == NULL) return -1;

    for (int i = 0; stringArray[i] != NULL; i++) {
        if (strcmp(stringArray[i], string) == 0) {
            return i;
        }
    }

    return -1;
}

bool is_background(const char *input) {
    size_t length = strlen(input);

    for (size_t i = 0; i < length; i++) {
        size_t index = length - 1 - i;
        if (input[index] == ' ') { // Ingores trailing white spaces
            continue;
        }
        return input[index] == BACKGROUND_OPERATOR;
    } // End: Reversed iteration over input
    return false;
}

/*
 * Sub-functions used to parse commands
 *
 * No declared in header file for privacy. Should not be called elsewhere.
 */

/**
 * @brief Predicates for the background property from the input
 *
 * @param input The raw user input
 * @return TRUE if the task should be run in the background; otherwise FALSE
 */
bool is_background(const char *input);

/**
 * @brief Parses the file and args from the input and set them in cmd
 *
 * Logs error messages on failure
 *
 * @param input The raw command input
 * @param cmd The command to set its file and args
 * @return 0 on success otherwise -1
 */
int parse_basic_cmd(const char *input, command_t *cmd);

/**
 * @brief Parses the output path of a command from its arguments
 *
 * @param cmd The command to parse
 */
void parse_cmd_output(command_t *cmd);

int parse_basic_cmd(const char *input, command_t *cmd) {
    char **tokArray = tokenize(input, ARG_SEPERATOR);
    if (tokArray == NULL) {
        log_f(ERROR, "Basic command parsing from '%s' has failed.", input);
        return -1;
    }

    if (tokArray[0] == NULL) {
        log_f(ERROR, "There is no file in '%s'.", input);
        delete_token_array(&tokArray);
        return -1;
    }

    set_file(cmd, tokArray[0]);
    set_args(cmd, tokArray);
    return 0;
}

void parse_cmd_output(command_t *cmd) {
    // Standard output redirection operator as a string
    const char output_operator[] = {OUTPUT_OPERATOR, '\0'};
    // File output parsing
    char **args = cmd->args;
    size_t args_length = length(args);

    if (args_length >= 2 &&
        strcmp(args[args_length - 2], output_operator) == 0) {
        cmd->output_path = args[args_length - 1];
        // Remove modifier and value from the list of arguments
        free(args[args_length - 2]);
        args[args_length - 2] = NULL;
        args[args_length - 1] = NULL;
    }
}

command_t *parse_cmd(const char *input, bool *p_is_bg) {
    char input_c[INPUT_SIZE];
    strcpy(input_c, input);
    *p_is_bg = false;
    bool is_bg = is_background(input_c);
    if (is_bg) str_replace(input_c, BACKGROUND_OPERATOR, ' ');
    // Piped command separator as a string
    const char pipe_separator[] = {PIPE_OPERATOR, '\0'};
    // Interrups parsing tasks and jumps to freeing memory
    bool has_error = false;
    // Input tokenized as piped command
    char **piped_input = tokenize(input_c, pipe_separator);
    if (piped_input == NULL) {
        log_f(ERROR, "Tokenizing '%s' as piped command as failed!", input_c);
        return NULL;
    }

    size_t piped_input_count = length(piped_input);
    // Command array
    command_t **cmd_array = calloc(sizeof(command_t *), piped_input_count);
    if (cmd_array == NULL) {
        log_err(ERROR, "Memory allocation for an array of commands has failed");
        delete_token_array(&piped_input);
        return NULL;
    }

    // Parses each command without linking piped output
    for (size_t i = 0; i < piped_input_count; i++) {
        if (is_background(piped_input[i])) {
            log_f(ERROR, "Cannot run in background a piped command!");
            has_error = true;
            break;
        }

        cmd_array[i] = new_command();
        int result = parse_basic_cmd(piped_input[i], cmd_array[i]);
        if (result == -1) {
            // On failed parsing exit function
            has_error = true;
            break;
        } else {
            parse_cmd_output(cmd_array[i]);
        }
    }
    // Links the piped output
    for (size_t i = 1; i < piped_input_count && !has_error; i++) {
        command_t *parent = cmd_array[piped_input_count - i - 1];
        command_t *child = cmd_array[piped_input_count - i];

        log_f(DEBUG, "Parent : %s", parent->file);
        if (child != NULL) {
            log_f(DEBUG, "Child : %s", child->file);
        } else {
            log_f(DEBUG, "Childless");
        }
        parent->child = child;
    }

    command_t *pipedCmd = NULL;
    delete_token_array(&piped_input);
    if (has_error) {
        for (size_t i = 0; cmd_array[i] != NULL; i++) {
            delete_command(&(cmd_array[i]));
        }
    } else {
        pipedCmd = cmd_array[0];
    }
    free(cmd_array);
    *p_is_bg = is_bg;
    return pipedCmd;
}
