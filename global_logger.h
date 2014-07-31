#ifndef __GLOBAL_LOGGER_H__
#define __GLOBAL_LOGGER_H__

/* global singleton logger implementation */

#include "logger.h"

/* see logger.h for flag definitions */
int log_init(const char* prefix, unsigned int flags,
             unsigned int max_megabytes);
void log_destroy(void);

#ifdef NDEBUG
#define log_debug(fmt, ...)
#else
#define log_debug(fmt, ...)     __log_debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#define log_user(fmt, ...)      __log_user(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)      __log_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)   __log_warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     __log_error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)     __log_fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

/* ------------------------------------------------------------------------- */

#ifndef NDEBUG
void __log_debug(const char* filename, int line, const char* fmt, ...);
#endif

void __log_user(const char* filename, int line, const char* fmt, ...);
void __log_info(const char* filename, int line, const char* fmt, ...);
void __log_warning(const char* filename, int line, const char* fmt, ...);
void __log_error(const char* filename, int line, const char* fmt, ...);
void __log_fatal(const char* filename, int line, const char* fmt, ...);

#endif
