#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

int level = 0;

void set_level(char lvl) { level = lvl; }

void _log(const char *msg, char lvl, const char *header)
{
    if (level <= lvl)
    {
        printf("%s %s\n",header, msg);
    }
}

void error(const char *msg) { _log(msg, ERROR, "[ERROR]"); }
void warn(const char *msg) { _log(msg, WARN, "[WARN]"); }
void info(const char *msg) { _log(msg, INFO, "[INFO]"); }
void debug(const char *msg) { _log(msg, DEBUG, "[DEBUG]"); }
