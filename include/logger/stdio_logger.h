#ifndef __LOGGER_STDIO_LOGGER_H__
#define __LOGGER_STDIO_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "logger.h"

struct stdio_logger {
    struct logger l;
    struct stdio_logger_impl* impl;
};

int stdio_logger_init(struct stdio_logger*);
void stdio_logger_destroy(struct stdio_logger*);

#ifdef __cplusplus
typedef struct stdio_logger StdioLogger;
}
#endif

#endif
