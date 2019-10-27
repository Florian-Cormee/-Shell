#include "main.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cd.h"
#include "command.h"
#include "logger.h"
#include "parser.h"

char g_cwd[WDIR_SIZE];

void print_cwd() {
    if (get_cwd(g_cwd, WDIR_SIZE) == NULL) {
        log_err(ERROR, "Could not get the current working directory");
        return;
    }
    printf("%s$ ", g_cwd);
}

int main(int argc, const char *argv[]) {
    // Sets the logger level from the arguments of the command
    log_level_t level = WARN;
    if (argc >= 2) {
        if (strcmp(argv[1], "--debug") == 0) {
            level = DEBUG;
        } else {
            printf("Usage: %s [--debug]\n--debug\tEnables debug logging.\n",
                   argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    set_log_level(level);

    // Initializes the program
    puts("Welcome in microShell!");
    const char chained_cmd_separator[] = {CHAINED_CMD_OPERATOR, '\0'};
    char input[INPUT_SIZE];
    memset(input, '\0', INPUT_SIZE); // Initialize input as an empty string
    print_cwd();
    bool pursue = true;

    while (pursue) {
        // Gets the input from stdin
        if (!fgets(input, INPUT_SIZE, stdin)) {
            log_err(ERROR, "Could not get your input");
            return -1;
        }
        str_replace(input, '\n', '\0');
        log_f(DEBUG, "Input: \"%s\"", input);

        // Tokenizes, parses and runs each chained command
        char **tok_array = tokenize(input, chained_cmd_separator);
        for (size_t i = 0; tok_array[i] != NULL; i++) {
            if (strncmp(EXIT_CMD, tok_array[i], strlen(EXIT_CMD)) == 0) {
                pursue = false;
            } else if (strncmp(CD_CMD, tok_array[i], strlen(CD_CMD)) == 0) {
                set_cwd(tok_array[i]);
            } else if (strncmp(PWD_CMD, tok_array[i], strlen(PWD_CMD)) == 0) {
                puts(g_cwd);
            } // End: Self implemented command
            else {
                // Parses the command out of the input
                bool is_bg;
                command_t *cmd = parse_cmd(tok_array[i], &is_bg);
                if (cmd != NULL) {
                    execute(cmd, !is_bg);
                    delete_command(&cmd);
                }
            }
        } // End: Iterating over chained commands
        delete_token_array(&tok_array);

        if (pursue) print_cwd();
    }
    return 0;
}
