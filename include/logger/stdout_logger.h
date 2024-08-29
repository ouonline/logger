#ifndef __LOGGER_STDOUT_LOGGER_H__
#define __LOGGER_STDOUT_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "logger.h"

struct stdout_logger {
    struct logger l;
    struct stdout_logger_impl* impl;
};

int stdout_logger_init(struct stdout_logger*);
void stdout_logger_destroy(struct stdout_logger*);

#ifdef __cplusplus
typedef struct stdout_logger StdoutLogger;
}
#endif

#endif
