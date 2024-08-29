#ifndef __LOGGER_LOGGER_H__
#define __LOGGER_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
    LOGGER_LEVEL_TRACE,
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARN,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_FATAL,
    LOGGER_LEVEL_MAX,
};

struct logger {
    /*
      log's level >= level will be recorded. default is `LOGGER_LEVEL_INFO` in
      release mode, and `LOGGER_LEVEL_TRACE` in debug mode. you can visit this
      variable directly, or use `logger_set_level()` and `logger_get_level()`.
    */
    unsigned int level;

    void (*func)(struct logger*, const char* filename, int line,
                 unsigned int level, const char* fmt, ...);
};

static inline void logger_set_level(struct logger* l, unsigned int level) {
    l->level = level;
}

static inline unsigned int logger_get_level(struct logger* l) {
    return l->level;
}

#define logger_trace(lp, fmt, ...) \
    (lp)->func(lp, __FILE__, __LINE__, LOGGER_LEVEL_TRACE, fmt, ##__VA_ARGS__)

#define logger_debug(lp, fmt, ...) \
    (lp)->func(lp, __FILE__, __LINE__, LOGGER_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define logger_info(lp, fmt, ...) \
    (lp)->func(lp, __FILE__, __LINE__, LOGGER_LEVEL_INFO, fmt, ##__VA_ARGS__)

#define logger_warn(lp, fmt, ...) \
    (lp)->func(lp, __FILE__, __LINE__, LOGGER_LEVEL_WARN, fmt, ##__VA_ARGS__)

#define logger_error(lp, fmt, ...) \
    (lp)->func(lp, __FILE__, __LINE__, LOGGER_LEVEL_ERROR, fmt, ##__VA_ARGS__)

#define logger_fatal(lp, fmt, ...) \
    (lp)->func(lp, __FILE__, __LINE__, LOGGER_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
typedef struct logger Logger;
}
#endif

#endif
