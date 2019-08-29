#ifndef __GLOBAL_LOGGER_H__
#define __GLOBAL_LOGGER_H__

/* global singleton logger implementation */

#include "logger.h"

/* see logger.h for flag definitions */
int log_init(const char* dirpath, const char* prefix, unsigned int flags,
             unsigned int max_megabytes);
void log_destroy(void);

#ifdef NDEBUG
#define log_debug(fmt, ...)
#else
#define log_debug(fmt, ...)     log_debug_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#define log_misc(fmt, ...)      log_misc_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)      log_info_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)   log_warning_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     log_error_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)     log_fatal_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

/* ------------------------------------------------------------------------- */

#ifndef NDEBUG
void log_debug_impl(const char* filename, int line, const char* fmt, ...);
#endif

void log_misc_impl(const char* filename, int line, const char* fmt, ...);
void log_info_impl(const char* filename, int line, const char* fmt, ...);
void log_warning_impl(const char* filename, int line, const char* fmt, ...);
void log_error_impl(const char* filename, int line, const char* fmt, ...);
void log_fatal_impl(const char* filename, int line, const char* fmt, ...);

#endif
