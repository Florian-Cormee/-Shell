#ifndef LOGGER_H
#define LOGGER_H

enum log_level { DEBUG, INFO, WARN, ERROR };
typedef enum log_level log_level_t;

void set_log_level(log_level_t lvl);
const char *get_header(log_level_t lvl);

/**
 * @brief Prints a debug message
 *
 * @param msg The message
 */
void debug(const char *msg);

/**
 * @brief Prints an info message
 *
 * @param msg The message
 */
void info(const char *msg);

/**
 * @brief Prints a warning message
 *
 * @param msg The message
 */
void warn(const char *msg);

/**
 * @brief Prints an error message in stderr
 *
 * @param msg The message
 */
void error(const char *msg);

/**
 * @brief Logs a formatted message
 *
 * @param lvl The message's level
 * @param format The message's format (c.f. printf)
 * @param ... The message's items (c.f. printf)
 */
void log_f(log_level_t lvl, const char *format, ...);

/**
 * @brief Logs a formatted message followed by errno corresponding description
 *
 * @param level The message's level
 * @param format The message's format (c.f. printf)
 * @param ... The message's items (c.f. printf)
 */
void log_err(log_level_t level, const char *format, ...);

/**
 * @brief Logs a formatted message followed by error_code corresponding
 * description
 *
 * @param level The message's level
 * @param error_code The error code to log
 * @param format The message's format (c.f. printf)
 * @param ... The message's items (c.f. printf)
 */
void log_error_code(log_level_t level, int error_code, const char *format, ...);
#endif // !LOGGER_H
