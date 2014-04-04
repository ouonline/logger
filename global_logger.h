#ifndef __GLOBAL_LOGGER_H__
#define __GLOBAL_LOGGER_H__

/* global singleton logger implementation */

#include "logger.h"

extern struct logger o_o;

int log_init(const char* prefix, unsigned flags, unsigned int max_megabytes);

#ifdef NDEBUG
#define log_debug(fmt, ...)
#else
#define log_debug(fmt, ...)     __logger_debug(&o_o, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#define log_user(fmt, ...)      __logger_user(&o_o, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)      __logger_info(&o_o, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)   __logger_warning(&o_o, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     __logger_error(&o_o, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)     __logger_fatal(&o_o, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif
