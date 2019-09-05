#ifndef __FILE_LOGGER_H__
#define __FILE_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "logger.h"

struct file_logger {
    struct logger l;
    struct file_logger_impl* impl;
};

/* flag definitions for init */
#define LOGGER_ROTATE_BY_SIZE       0x1
#define LOGGER_ROTATE_PER_HOUR      0x2
#define LOGGER_ROTATE_PER_DAY       0x4
#define LOGGER_ROTATE_DEFAULT       LOGGER_ROTATE_PER_DAY

int file_logger_init(struct file_logger*, const char* dirpath, const char* prefix,
                     unsigned int flags, unsigned int max_megabytes);
void file_logger_destroy(struct file_logger*);

#ifdef __cplusplus
}
#endif

#endif
