#include "command.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "logger.h"
#include "main.h"
#include "parser.h"

command_t *new_command() {
    command_t *pcmd = calloc(sizeof(command_t), 1);
    if (pcmd == NULL) {
        log_err(ERROR, "Memory allocation for a new command has failed");
    }
    return pcmd;
}

void delete_command(command_t **p_command) {
    // Test for NULL input
    if (p_command == NULL || *p_command == NULL) {
        return;
    }
    // Frees every non-NULL fields
    command_t *command = *p_command;
    if (command->file != NULL) {
        free(command->file);
        command->file = NULL;
    }
    if (command->args != NULL) {
        for (size_t i = 0; command->args[i] != NULL; i++) {
            free(command->args[i]);
        }
        free(command->args);
        command->args = NULL;
    }
    if (command->child != NULL) {
        delete_command(&(command->child));
    }
    if (command->output_path != NULL) {
        free(command->output_path);
    }
    // Frees the command itself
    free(command);
    *p_command = NULL;
}

void set_file(command_t *cmd, const char *file) {
    if (cmd == NULL) return;
    if (cmd->file != NULL) free(cmd->file);
    cmd->file = file == NULL ? NULL : strdup(file);
}

void set_args(command_t *cmd, char **args) {
    if (cmd == NULL) return;
    if (cmd->args != NULL) delete_token_array(&(cmd->args));
    cmd->args = args;
}

void set_output(command_t *cmd, const char *path) {
    if (cmd == NULL) return;
    if (cmd->output_path != NULL) free(cmd->output_path);
    cmd->output_path = strdup(path);
}

void set_child(command_t *cmd, command_t *child) {
    if (cmd == NULL) return;
    if (cmd->child != NULL) delete_command(&(cmd->child));
    cmd->child = child;
}

/*
 * Execute's utilities functions decleration.
 *
 * Not declared in header for privacy. Should not be called anywhere else.
 */

/**
 * @brief Create a enough pipes to execute the given command
 *
 * Logs error message on failure
 *
 * @param command The command; can not be NULL
 * @param max_pipes_count The maximum amount of pipes to create
 * @param pipes_count The actual amount of pipes created
 * @return An array of size (pipes_count, 2) of file descriptor; on failure:
 * NULL
 */
int **
create_pipes(command_t *command, size_t max_pipes_count, size_t *pipes_count);

/**
 * @brief Closes all pipes in an array of size (amount, 2)
 *
 * Frees both end of the each pipes
 * Frees the sub-array and replaces it by NULL
 *
 * @param pipefd The array of pipes to close
 * @param amount The length of pipefd
 */
void close_all_pipes(int **pipefd, size_t amount);

/**
 * @brief Set the standard IO for this command
 *
 * Logs error messages on failure
 *
 * @param cmd The command
 * @param generation The generation of the command (as in a family tree)
 * @param pipefd The array of pipes
 * @param pipes_count The size of the array
 */
int set_std_io(command_t *cmd,
               size_t generation,
               int **pipefd,
               size_t pipes_count);

/**
 * @brief Wait for every given child processes
 *
 * Logs error messages on failure
 *
 * @param pids An array of every child processe's pid to wait for
 * @param pids_count The size of the array
 * @return On success 0; otherwise -1
 */
int wait_for_children(int *pids, size_t pids_count);

int **
create_pipes(command_t *command, size_t max_pipes_count, size_t *pipes_count) {
    int **pipefd = calloc(sizeof(int *), max_pipes_count);
    if (pipefd == NULL) {
        log_err(ERROR, "Memory allocation for an array of pipes has failed");
        return NULL;
    }

    for (command_t *current = command;
         current != NULL && current->child != NULL;
         current = current->child) {
        if ((*pipes_count) >= max_pipes_count) {
            log_f(ERROR,
                  "Out of bounds (aka %d) amount of pipes.",
                  max_pipes_count);
            // Frees already created pipes
            close_all_pipes(pipefd, *pipes_count);
            free(pipefd);
            return NULL;
        } // End: Maximum pipe amount reached

        debug("Creates new pipe!");

        pipefd[*pipes_count] = calloc(sizeof(int), 2);
        if (pipefd[*pipes_count] == NULL) {
            log_err(ERROR,
                    "Memory allocation for pipe n°%d has failed",
                    *pipes_count);
            // Frees already created pipes
            close_all_pipes(pipefd, *pipes_count);
            free(pipefd);
            return NULL;
        }

        if (pipe(pipefd[*pipes_count]) == -1) {
            log_err(ERROR, "Creation of pipe n°%d has failed", *pipes_count);
            // Frees already created pipes
            close_all_pipes(pipefd, *pipes_count);
            free(pipefd);
            return NULL;
        }

        (*pipes_count)++;
    } // End: Iterating over commands outputing to pipes
    return pipefd;
}

void close_all_pipes(int **pipefd, size_t amount) {
    for (size_t i = 0; i < amount; i++) {
        if (pipefd[i] != NULL) {
            close(pipefd[i][PIPE_IN]);
            close(pipefd[i][PIPE_OUT]);
            free(pipefd[i]);
            pipefd[i] = NULL;
        }
    }
}

int set_std_io(command_t *cmd,
               size_t generation,
               int **pipefd,
               size_t pipes_count) {
    log_f(DEBUG, "generation: %ld", generation);
    log_f(DEBUG, "pipes_cout: %ld", pipes_count);

    if (generation > 0 && generation - 1 < pipes_count) {
        log_f(INFO, "%s : redirecting the input.", cmd->file);
        if (dup2(pipefd[generation - 1][PIPE_IN], STDIN_FILENO) == -1) {
            log_err(ERROR, "'%s' input redirection has failed", cmd->file);
            return -1;
        }
    }

    if (cmd->output_path != NULL) {
        log_f(INFO,
              "%s : redirecting the output to %s.",
              cmd->file,
              cmd->output_path);
        FILE *f = fopen(cmd->output_path, "w");
        if (f == NULL) {
            log_err(ERROR, "'%s' is an invalid path", cmd->output_path);
            return -1;
        }

        int fd = fileno(f);
        int r_dup2 = dup2(fd, STDOUT_FILENO);
        fclose(f);
        if (r_dup2 == -1) {
            log_err(ERROR,
                    "Output redirection of the command '%s' has failed",
                    cmd->file);
            return -1;
        }
    } // End: Output redirection to a path
    else if (cmd->child != NULL && generation < pipes_count) {
        log_f(INFO, "%s : redirecting the output to a pipe.", cmd->file);
        if (dup2(pipefd[generation][PIPE_OUT], STDOUT_FILENO) == -1) {
            log_err(ERROR,
                    "Output redirection of the command '%s' has failed",
                    cmd->file);
            return -1;
        }
    } // End: Output redirection to a pipe
    return 0;
}

int wait_for_children(int *pids, size_t pids_count) {
    for (size_t i = 0; i < pids_count; i++) {
        log_f(INFO, "Waiting for child %ld / %ld..", i + 1, pids_count);
        int wstatus;
        int r_wait = waitpid(pids[i], &wstatus, 0);

        log_f(DEBUG,
              "Waiting done : %d has exited normally %d, exit status: %d, has "
              "signaled: %d, signal: %d",
              r_wait,
              WIFEXITED(wstatus),
              WEXITSTATUS(wstatus),
              WIFSIGNALED(wstatus),
              WTERMSIG(wstatus));
        if (!WIFEXITED(wstatus)) { // If the child raised a signal
            log_error_code(ERROR, WTERMSIG(wstatus), "Execution has failed");
        }

        if (r_wait == -1) {
            log_f(WARN,
                  "Waiting for child %ld / %ld has failed",
                  i + 1,
                  pids_count);
            return -1;
        }
    }
    log_f(INFO, "Done waiting.");
    return 0;
}

int execute(command_t *command, bool wait_children) {
    const size_t max_pipes_count = 16;
    size_t pipes_count = 0;
    int **pipefd = create_pipes(command, max_pipes_count, &pipes_count);
    pid_t pids[max_pipes_count + 1];

    if (pipefd == NULL) {
        return -1;
    }

    // Creates child processes
    command_t *current = command;
    size_t pids_count = 0;

    while (current != NULL && pids_count < max_pipes_count + 1) {
        log_f(DEBUG, "Current: %s", current == NULL ? "None" : current->file);

        pids[pids_count] = fork();

        if (pids[pids_count] == -1) {
            log_err(ERROR, "Fork n°%d has failed", pids_count);
            close_all_pipes(pipefd, pipes_count);
            return -1;
        } // End: Error handling
        else if (pids[pids_count] == 0) {
            int r_ssio = set_std_io(current, pids_count, pipefd, pipes_count);
            close_all_pipes(pipefd, pipes_count);
            if (r_ssio != -1) {
                execvp(current->file, current->args);
                log_err(ERROR, "Execution of '%s' has failed", current->file);
            }
            exit(EXIT_FAILURE);
            return -1;
        } // End: child process
        else {
            log_f(DEBUG,
                  "New child process for command: '%s' has pid: %d",
                  current->file,
                  pids[pids_count]);
        } // End: parent process
        current = current->child;
        pids_count++;
    }

    // Closes pipes
    close_all_pipes(pipefd, pipes_count);
    free(pipefd);

    if (wait_children) {
        if (wait_for_children(pids, pids_count) == -1) {
            return -1;
        }
    }
    return 0;
}
