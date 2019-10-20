#ifndef LOGGER_H
#define LOGGER_H

#define ERROR 8
#define WARN 4
#define INFO 2
#define DEBUG 1

void set_level(char lvl);

void error(const char* msg);
void warn(const char* msg);
void info(const char* msg);
void debug(const char* msg);
#endif // !LOGGER_H
