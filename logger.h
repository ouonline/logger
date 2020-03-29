#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

struct logger {
    const struct logger_operations* ops;
};

typedef void (*logger_func_t)(struct logger*, const char* filename, int line,
                              const char* funcname, const char* fmt, ...);

struct logger_operations {
    logger_func_t debug;
    logger_func_t misc;
    logger_func_t info;
    logger_func_t warning;
    logger_func_t error;
    logger_func_t fatal;
};

#ifdef NDEBUG
#define logger_debug(lp, fmt, ...)
#else
#define logger_debug(lp, fmt, ...)      (lp)->ops->debug(lp, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif

#define logger_misc(lp, fmt, ...)       (lp)->ops->misc(lp, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define logger_info(lp, fmt, ...)       (lp)->ops->info(lp, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define logger_warning(lp, fmt, ...)    (lp)->ops->warning(lp, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define logger_error(lp, fmt, ...)      (lp)->ops->error(lp, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define logger_fatal(lp, fmt, ...)      (lp)->ops->fatal(lp, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
