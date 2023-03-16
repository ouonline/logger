#ifndef __LOGGER_LOGGER_H__
#define __LOGGER_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGER_LEVEL_DEBUG     0
#define LOGGER_LEVEL_INFO      1
#define LOGGER_LEVEL_WARNING   2
#define LOGGER_LEVEL_ERROR     3
#define LOGGER_LEVEL_FATAL     4
#define LOGGER_LEVEL_MAX       5

struct logger {
    unsigned int level; /* logs >= level will be recorded. */
    const struct logger_operations* ops;
};

#ifdef __cplusplus
typedef struct logger Logger;
#endif

typedef void (*logger_func_t)(struct logger*, const char* filename, int line,
                              const char* fmt, ...);

struct logger_operations {
    logger_func_t debug;
    logger_func_t info;
    logger_func_t warning;
    logger_func_t error;
    logger_func_t fatal;
};

#ifdef NDEBUG
#define logger_debug(lp, fmt, ...)
#else
#define logger_debug(lp, fmt, ...)      (lp)->ops->debug(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#define logger_info(lp, fmt, ...)       (lp)->ops->info(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_warning(lp, fmt, ...)    (lp)->ops->warning(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_error(lp, fmt, ...)      (lp)->ops->error(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_fatal(lp, fmt, ...)      (lp)->ops->fatal(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
