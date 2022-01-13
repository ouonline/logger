#include "logger/file_logger.h"
#include "cutils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

/* ------------------------------------------------------------------------- */

#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_MISC      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_WARNING   3
#define LOG_LEVEL_ERROR     4
#define LOG_LEVEL_FATAL     5
#define LOG_LEVEL_MAX       6

#define LOGGER_ROTATE_FLAG_MASK     0x7

static const char* log_level_str[] = {
    "debug", "misc", "info", "warning", "error", "fatal",
};

/* fields of logging timestamp */
struct log_tm {
    unsigned short tm_year;
    unsigned short tm_mon;
    unsigned short tm_mday;
    unsigned short tm_hour;
} __attribute__((packed));

/* log filename may be one of the following forms:
 *     - <prefix>_YYYYMMDD.<level>
 *     - <prefix>_YYYYMMDD-hh.<level>
 *     - <prefix>_YYYYMMDD-hhmmss.<level>
 *
 * strlen("_YYYYMMDD_hhmmss.") == 17
 * max(strlen(log_level_str[])) == strlen("warning") == 7
 *
 * PATH_BUFLEN should be >= 17 + 7 + 1 = 25 (including trailing '\0') */
#define MIN_PATH_BUFLEN 25

#define PATH_BUFLEN 1024

#define PATH_PREFIX_BUFLEN  (PATH_BUFLEN \
                             - sizeof(unsigned long) \
                             - sizeof(void*) - sizeof(void*))

/* variables that will be initialized in file_logger_init() */
struct logger_var {
    unsigned long max_file_size;

    int (*rotate_trigger)(const struct tm* current, const struct log_tm* old,
                          unsigned long max_file_size, unsigned long filesize);
    void (*get_filename)(char* buf, int level, const struct tm* ts);

    char path_prefix[PATH_PREFIX_BUFLEN];
} __attribute__((packed));

#define MAX_LOG_LEN 4096

struct logger_info {
    FILE* fp;
    struct log_tm ts; /* timestamp of last write */
    unsigned int level;
    unsigned long filesize;
    pthread_mutex_t lock;
    char buf[MAX_LOG_LEN];
};

struct file_logger_impl {
    struct logger_var var;
    struct logger_info o_o[LOG_LEVEL_MAX];
};

/* ------------------------------------------------------------------------- */

static void __new_log_level_file(struct logger_info* logger,
                                 struct logger_var* var,
                                 const struct tm* ts) {
    int len;
    char path[PATH_BUFLEN];

    logger->filesize = 0;

    if (logger->fp && logger->fp != stdout && logger->fp != stderr) {
        fclose(logger->fp);
    }

    len = sprintf(path, "%s_", var->path_prefix);
    var->get_filename(path + len, logger->level, ts);

    logger->fp = fopen(path, "a");
    if (!logger->fp) {
        fprintf(stderr, "cannot create/open file `%s', "
                "log content of level [%s] is redirected to %s.\n",
                path, log_level_str[logger->level],
                (logger->level <= LOG_LEVEL_INFO) ? "stdout" : "stderr");
        logger->fp = (logger->level <= LOG_LEVEL_INFO) ? stdout : stderr;
    }
}

/* time format: YYYY-MM-DD hh:mm:ss.uuuuuu
 * buf size >= 27 */
static void current_datetime(char buf[], struct tm* tp) {
    int len;
    struct timeval tv;
    struct tm ltm;

    if (!tp) {
        tp = &ltm;
    }

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, tp);
    len = strftime(buf, 27, "%F %T", tp);
    sprintf((char*)buf + len, ".%06ld", tv.tv_usec);
}

static void generic_logger(struct logger_info* logger, struct logger_var* var,
                           const char* filename, int line, /* extra info */
                           const char* fmt, va_list* args) {
    struct tm tm;
    char timestr[32];

    pthread_mutex_lock(&logger->lock);

    current_datetime(timestr, &tm);

    if (var->rotate_trigger(&tm, &logger->ts, var->max_file_size,
                            logger->filesize)) {
        __new_log_level_file(logger, var, &tm);

        logger->ts.tm_hour = tm.tm_hour;
        logger->ts.tm_mday = tm.tm_mday;
        logger->ts.tm_mon = tm.tm_mon;
        logger->ts.tm_year = tm.tm_year;
    }

    vsnprintf(logger->buf, MAX_LOG_LEN, fmt, *args);
    logger->filesize += fprintf(logger->fp, "[%s] [%s:%u]\t%s\n",
                                timestr, filename, line, logger->buf);
    fflush(logger->fp); /* flush cache to disk */

    pthread_mutex_unlock(&logger->lock);
}

static inline void __vlogger(struct logger* l, int level,
                             const char* filename, int line,
                             const char* fmt, va_list* args) {
    struct file_logger* fl = container_of(l, struct file_logger, l);
    struct file_logger_impl* impl = fl->impl;
    generic_logger(&impl->o_o[level], &impl->var, filename, line, fmt, args);
}

void file_logger_debug(struct logger* l, const char* filename, int line,
                       const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __vlogger(l, LOG_LEVEL_DEBUG, filename, line, fmt, &args);
    va_end(args);
}

void file_logger_misc(struct logger* l, const char* filename, int line,
                      const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __vlogger(l, LOG_LEVEL_MISC, filename, line, fmt, &args);
    va_end(args);
}

void file_logger_info(struct logger* l, const char* filename, int line,
                      const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __vlogger(l, LOG_LEVEL_INFO, filename, line, fmt, &args);
    va_end(args);
}

void file_logger_warning(struct logger* l, const char* filename, int line,
                         const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __vlogger(l, LOG_LEVEL_WARNING, filename, line, fmt, &args);
    va_end(args);
}

void file_logger_error(struct logger* l, const char* filename, int line,
                       const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __vlogger(l, LOG_LEVEL_ERROR, filename, line, fmt, &args);
    va_end(args);
}

void file_logger_fatal(struct logger* l, const char* filename, int line,
                       const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __vlogger(l, LOG_LEVEL_FATAL, filename, line, fmt, &args);
    va_end(args);
}

/* ------------------------------------------------------------------------- */

/* all possible combinations of rotating conditions */

static inline int trigger_size(const struct tm* current,
                               const struct log_tm* old,
                               unsigned long max_file_size,
                               unsigned long filesize) {
    (void)current;
    (void)old;
    return (filesize >= max_file_size);
}

static inline void filename_size(char* buf, int level, const struct tm* ts) {
    sprintf(buf, "%04d%02d%02d-%02d%02d%02d.%s",
            ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday,
            ts->tm_hour, ts->tm_min, ts->tm_sec,
            log_level_str[level]);
}

static inline int trigger_hour(const struct tm* current,
                               const struct log_tm* old,
                               unsigned long max_file_size,
                               unsigned long filesize) {
    (void)max_file_size;
    (void)filesize;
    return (! ((old->tm_hour == current->tm_hour) &&
               (old->tm_mday == current->tm_mday) &&
               (old->tm_mon == current->tm_mon) &&
               (old->tm_year == current->tm_year)));
}

static inline void filename_hour(char* buf, int level, const struct tm* ts) {
    sprintf(buf, "%04d%02d%02d-%02d.%s",
            ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday, ts->tm_hour,
            log_level_str[level]);
}

static inline int trigger_day(const struct tm* current,
                              const struct log_tm* old,
                              unsigned long max_file_size,
                              unsigned long filesize) {
    (void)max_file_size;
    (void)filesize;
    return (! ((old->tm_mday == current->tm_mday) &&
               (old->tm_mon == current->tm_mon) &&
               (old->tm_year == current->tm_year)));
}

static inline void filename_day(char* buf, int level, const struct tm* ts) {
    sprintf(buf, "%04d%02d%02d.%s",
            ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday,
            log_level_str[level]);
}

static inline int trigger_size_hour(const struct tm* current,
                                    const struct log_tm* old,
                                    unsigned long max_file_size,
                                    unsigned long filesize) {
    return (trigger_size(current, old, max_file_size, filesize) ||
            trigger_hour(current, old, max_file_size, filesize));
}

static inline void filename_size_hour(char* buf, int level, const struct tm* ts) {
    filename_size(buf, level, ts);
}

static inline int trigger_size_day(const struct tm* current,
                                   const struct log_tm* old,
                                   unsigned long max_file_size,
                                   unsigned long filesize) {
    return (trigger_size(current, old, max_file_size, filesize) ||
            trigger_day(current, old, max_file_size, filesize));
}

static inline void filename_size_day(char* buf, int level, const struct tm* ts) {
    filename_size(buf, level, ts);
}

static inline int trigger_none(const struct tm* current,
                               const struct log_tm* old,
                               unsigned long max_file_size,
                               unsigned long filesize) {
    (void)current;
    (void)old;
    (void)max_file_size;
    (void)filesize;
    return 0;
}

static inline void filename_none(char* buf, int level, const struct tm* ts) {
    (void)buf;
    (void)level;
    (void)ts;
}

static void logger_var_set_func(struct logger_var* var, unsigned flags) {
    switch (flags & LOGGER_ROTATE_FLAG_MASK) {
        case LOGGER_ROTATE_BY_SIZE:
            var->rotate_trigger = trigger_size;
            var->get_filename = filename_size;
        break;

        case LOGGER_ROTATE_PER_HOUR:
        case LOGGER_ROTATE_PER_HOUR | LOGGER_ROTATE_PER_DAY:
            var->rotate_trigger = trigger_hour;
            var->get_filename = filename_hour;
        break;

        case LOGGER_ROTATE_BY_SIZE | LOGGER_ROTATE_PER_HOUR:
        case LOGGER_ROTATE_BY_SIZE | LOGGER_ROTATE_PER_HOUR | LOGGER_ROTATE_PER_DAY:
            var->rotate_trigger = trigger_size_hour;
            var->get_filename = filename_size_hour;
        break;

        case LOGGER_ROTATE_BY_SIZE | LOGGER_ROTATE_PER_DAY:
            var->rotate_trigger = trigger_size_day;
            var->get_filename = filename_size_day;
        break;

        case LOGGER_ROTATE_PER_DAY:
        default:
            var->rotate_trigger = trigger_day;
            var->get_filename = filename_day;
    }
}

static inline void logger_var_set_path_prefix(struct logger_var* var,
                                              const char* dirpath, const char* prefix,
                                              unsigned int max_prefix_len) {
    unsigned int path_prefix_len = strlen(dirpath) + 1 /* '/' */ + strlen(prefix);

    if (path_prefix_len > max_prefix_len) {
        fprintf(stderr, "prefix len is greater than %d, truncated.\n",
                max_prefix_len);
        path_prefix_len = max_prefix_len;
    }

    snprintf(var->path_prefix, path_prefix_len + 1 /* plus trailing '\0' */,
             "%s/%s", dirpath, prefix);
}

static inline void logger_var_init(struct logger_var* var,
                                   const char* dirpath, const char* prefix,
                                   unsigned int flags,
                                   unsigned int max_megabytes) {
    logger_var_set_func(var, flags);
    var->max_file_size = max_megabytes << 20;
    logger_var_set_path_prefix(var, dirpath, prefix,
                               (PATH_PREFIX_BUFLEN > PATH_BUFLEN - MIN_PATH_BUFLEN) ?
                               (PATH_BUFLEN - MIN_PATH_BUFLEN - 1) :
                               (PATH_PREFIX_BUFLEN - 1));
}

/* ------------------------------------------------------------------------- */

#include <sys/stat.h>
#include <errno.h>

static const struct logger_operations file_logger_operations = {
    .debug = file_logger_debug,
    .misc = file_logger_misc,
    .info = file_logger_info,
    .warning = file_logger_warning,
    .error = file_logger_error,
    .fatal = file_logger_fatal,
};

int file_logger_init(struct file_logger* l, const char* dirpath, const char* prefix,
                     unsigned int flags, unsigned int max_megabytes)
{
    unsigned int i;
    struct file_logger_impl* impl;

    if (!dirpath || !prefix) {
        fprintf(stderr, "empty `dirpath` or `prefix`.\n");
        return -1;
    }

    if (mkdir(dirpath, 0755) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "logger init: mkdir(%s) failed: %s.",
                    dirpath, strerror(errno));
            return -1;
        }
    }

    impl = (struct file_logger_impl*)malloc(sizeof(struct file_logger_impl));
    if (!impl) {
        return -1;
    }

    memset(impl, 0, sizeof(struct file_logger));

    for (i = 0; i < LOG_LEVEL_MAX; ++i) {
        struct logger_info* logger = &impl->o_o[i];
        logger->level = i;
        pthread_mutex_init(&logger->lock, NULL);
    }

    logger_var_init(&impl->var, dirpath, prefix, flags, max_megabytes);

    l->impl = impl;
    l->l.ops = &file_logger_operations;
    return 0;
}

void file_logger_destroy(struct file_logger* l) {
    unsigned int i;
    struct file_logger_impl* impl = l->impl;

    if (!impl) {
        return;
    }

    for (i = 0; i < LOG_LEVEL_MAX; ++i) {
        struct logger_info* logger = &impl->o_o[i];

        if (logger->fp && logger->fp != stdout && logger->fp != stderr) {
            fclose(logger->fp);
        }

        pthread_mutex_destroy(&logger->lock);
    }

    free(impl);
    l->impl = NULL;
}
