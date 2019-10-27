#include "cd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include "logger.h"
#include "main.h"
#include "parser.h"
#include "utils.h"

char *get_cwd(char *wdir, size_t size) {
    getcwd(wdir, size);

    if (strcmp(getenv("HOME"), wdir) == 0) {
        strcpy(wdir, "~");
    }
    return wdir;
}

int set_cwd(const char *input) {
    debug("Entering set_cwd");

    char **argv = tokenize(input, ARG_SEPERATOR);
    for (size_t i = 0; argv[i] != NULL; i++) {
        log_f(DEBUG, "argv[%d]: %s", i, argv[i]);
    }

    char *path = argv[1];
    if (path == NULL) {
        // No path specified; Setting home directory
        path = getenv("HOME");
    }
    int r_chdir = chdir(path);

    // Invalid path message
    if (r_chdir == -1) {
        log_err(ERROR, "Invalid path: '%s'", path);
    }
    delete_token_array(&argv);
    return r_chdir;
}
