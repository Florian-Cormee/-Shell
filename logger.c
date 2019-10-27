#include "logger.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

log_level_t g_level = 0;

void set_log_level(log_level_t lvl) {
    g_level = lvl;
}

const char *get_header(log_level_t lvl) {
    switch (lvl) {
    case DEBUG:
        return "[DEBUG]";
    case INFO:
        return "[INFO]";
    case WARN:
        return "[WARN]";
    case ERROR:
        return "[ERROR]";
    default:
        break;
    }
    return "";
}

void _log(log_level_t lvl, const char *msg) {
    FILE *output = lvl < ERROR ? stdout : stderr;
    if (g_level <= lvl) {
        fprintf(output, "%s %s\n", get_header(lvl), msg);
    }
}

void debug(const char *msg) {
    _log(DEBUG, msg);
}
void info(const char *msg) {
    _log(INFO, msg);
}
void warn(const char *msg) {
    _log(WARN, msg);
}
void error(const char *msg) {
    _log(ERROR, msg);
}

void log_f(log_level_t lvl, const char *format, ...) {
    if (lvl < g_level) {
        return;
    }

    const char *header = get_header(lvl);
    FILE *output = lvl < ERROR ? stdout : stderr;
    va_list items;

    fprintf(output, "%s ", header);

    va_start(items, format);
    vfprintf(output, format, items);
    va_end(items);
    fputc('\n', output);
}

void log_errno(log_level_t level, int error_code, const char *format, va_list items) {
    if (level < g_level) {
        return;
    }
    const char *header = get_header(level);
    FILE *output = level < ERROR ? stdout : stderr;

    fprintf(output, "%s ", header);

    vfprintf(output, format, items);
    fprintf(output, ": %s\n", strerror(error_code));
}

void log_err(log_level_t level, const char *format, ...) {
    int error_code = errno;
    va_list items;
    va_start(items, format);
    log_errno(level, error_code, format, items);
    va_end(items);
}

void log_error_code(log_level_t level, int error_code, const char *format, ...) {
    va_list items;
    va_start(items, format);
    log_errno(level, error_code,format, items);
    va_end(items);
}

/* Testing area */
/*
int main(int argc, char const *argv[]) {
    set_log_level(INFO);

    debug("Should not be seen");
    log_f(DEBUG, "Should not be seen %s\n", "formatted");

    info("Should be seen");
    log_f(INFO, "Should be seen %s\n", "formatted");

    warn("Should be seen");
    log_f(WARN, "Should be seen %s\n", "formatted");

    error("Should be seen");
    log_f(ERROR, "Should be seen %s\n", "formatted");
    return 0;
}
*/
