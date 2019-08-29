#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* flag definitions for logger_init() */
#define LOGGER_ROTATE_BY_SIZE       0x1
#define LOGGER_ROTATE_PER_HOUR      0x2
#define LOGGER_ROTATE_PER_DAY       0x4
#define LOGGER_ROTATE_DEFAULT       LOGGER_ROTATE_PER_DAY

#define LOGGER_ROTATE_FLAG_MASK     0x7

/* ------------------------------------------------------------------------- */

struct logger {
    struct logger_impl* handler;
};

int logger_init(struct logger*, const char* dirpath, const char* prefix,
                unsigned int flags, unsigned int max_megabytes);
void logger_destroy(struct logger*);

#ifdef NDEBUG
#define logger_debug(lp, fmt, ...)
#else
#define logger_debug(lp, fmt, ...)      logger_debug_impl(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#define logger_misc(lp, fmt, ...)       logger_misc_impl(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_info(lp, fmt, ...)       logger_info_impl(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_warning(lp, fmt, ...)    logger_warning_impl(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_error(lp, fmt, ...)      logger_error_impl(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define logger_fatal(lp, fmt, ...)      logger_fatal_impl(lp, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/* ------------------------------------------------------------------------- */

#ifndef NDEBUG
void logger_debug_impl(struct logger*, const char* filename, int line,
                       const char* fmt, ...);
#endif

void logger_misc_impl(struct logger*, const char* filename, int line,
                      const char* fmt, ...);
void logger_info_impl(struct logger*, const char* filename, int line,
                      const char* fmt, ...);
void logger_warning_impl(struct logger*, const char* filename, int line,
                         const char* fmt, ...);
void logger_error_impl(struct logger*, const char* filename, int line,
                       const char* fmt, ...);
void logger_fatal_impl(struct logger*, const char* filename, int line,
                       const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
