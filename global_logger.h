#ifndef __GLOBAL_LOGGER_H__
#define __GLOBAL_LOGGER_H__

/* global singleton logger implementation */

#include "logger.h"

extern struct logger o_o;

/* see logger.h for flag definitions */
#define log_init(prefix, flags, max_megabytes) \
    logger_init(&o_o, prefix, flags, max_megabytes)

#define log_debug(fmt, ...)     logger_debug(&o_o, fmt, ##__VA_ARGS__)
#define log_user(fmt, ...)      logger_user(&o_o, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)      logger_info(&o_o, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)   logger_warning(&o_o, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     logger_error(&o_o, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)     logger_fatal(&o_o, fmt, ##__VA_ARGS__)

#endif
