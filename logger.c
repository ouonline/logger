#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "logger.h"

/* ------------------------------------------------------------------------- */

#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_USER      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_WARNING   3
#define LOG_LEVEL_ERROR     4
#define LOG_LEVEL_FATAL     5
#define LOG_LEVEL_MAX       6

static const char* log_level_str_lower[] = {
    "debug", "user", "info", "warning", "error", "fatal",
};

struct log_tm {
    unsigned tm_year : 16;
    unsigned tm_mon : 8;
    unsigned tm_mday : 8;
    unsigned tm_hour : 8;
    unsigned padding : 24;
} __attribute__((packed));

#define MAX_LOG_LEN (4096 \
                     - sizeof(FILE*) \
                     - sizeof(struct log_tm) \
                     - sizeof(unsigned long) \
                     - sizeof(pthread_mutex_t))

static struct {
    FILE* fp;
    struct log_tm ts; /* timestamp of last write */
    unsigned long filesize;
    pthread_mutex_t lock;
    char buf[MAX_LOG_LEN];
} __attribute__((packed)) g_[LOG_LEVEL_MAX];

/* log filename may be one of the following forms:
 *     - <prefix>YYYY-MM-DD.<level>
 *     - <prefix>YYYY-MM-DD_hh.<level>
 *     - <prefix>YYYY-MM-DD_hh:mm:ss.<level>
 * max(strlen(level_str[])) == strlen("warning") == 7 */

#define PATH_BUFLEN             1024 /* >= 28 */
#define MAX_PATH_LEN            (PATH_BUFLEN - 1)
#define MAX_PATH_PREFIX_LEN     (MAX_PATH_LEN - 27)

static unsigned long g_max_file_size;

static int g_path_prefix_len;
static char g_path[PATH_BUFLEN];

int (*g_rotate_trigger)(const struct tm* current, const struct log_tm* old,
                        unsigned long filesize);

void (*g_get_filename)(char* buf, int level, const struct tm* ts);

/* ------------------------------------------------------------------------- */

static inline void __new_log_level_file(int level, const struct tm* ts)
{
    g_[level].filesize = 0;

    if (g_[level].fp && g_[level].fp != stdout && g_[level].fp != stderr)
        fclose(g_[level].fp);

    g_get_filename(g_path + g_path_prefix_len, level, ts);

    g_[level].fp = fopen(g_path, "a");
    if (!g_[level].fp) {
        fprintf(stderr, "cannot create/open file `%s', "
                "log content of level [%s] is redirected to %s.\n",
                g_path, log_level_str_lower[level],
                (level == LOG_LEVEL_INFO) ? "stdout" : "stderr");
        g_[level].fp = (level == LOG_LEVEL_INFO) ? stdout : stderr;
    }
}

/* time format: YYYY-MM-DD hh:mm:ss.uuuuuu
 * size >= 27 */
static inline void current_datetime(char *buf, int size, struct tm* tp)
{
    int len;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, tp);
    len = strftime(buf, size, "%F %T", tp);
    sprintf(buf + len, ".%06ld", tv.tv_usec);
}

#include <unistd.h>
#include <sys/syscall.h>

static void generic_logger(int level, const char* extra_info,
                           const char* fmt, va_list args)
{
    int len;
    struct tm tm;
    char timestr[32];

    current_datetime(timestr, 32, &tm);

    pthread_mutex_lock(&g_[level].lock);

    if (g_rotate_trigger(&tm, &g_[level].ts, g_[level].filesize)) {
        __new_log_level_file(level, &tm);

        g_[level].ts.tm_hour = tm.tm_hour;
        g_[level].ts.tm_mday = tm.tm_mday;
        g_[level].ts.tm_mon = tm.tm_mon;
        g_[level].ts.tm_year = tm.tm_year;
    }

    len = sprintf(g_[level].buf, "%s %ld %s\t", timestr,
                  syscall(__NR_gettid), extra_info);
    vsnprintf(g_[level].buf + len, MAX_LOG_LEN - len, fmt, args);
    len = fprintf(g_[level].fp, "%s\n", g_[level].buf);

    g_[level].filesize += len;

#ifndef NDEBUG
    if (level == LOG_LEVEL_DEBUG)
        fprintf(stdout, "%s %s\n", log_level_str_lower[level], g_[level].buf);
    else if (level >= LOG_LEVEL_WARNING)
        fprintf(stderr, "%s %s\n", log_level_str_lower[level], g_[level].buf);
#endif

    pthread_mutex_unlock(&g_[level].lock);
}

void __log_user(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_USER, "", fmt, args);
    va_end(args);
}

void __log_info(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_INFO, "", fmt, args);
    va_end(args);
}

#define MAX_POS_INFO_LEN 128

static inline void log_with_pos_info(int level, const char* filename, int line,
                                     const char* fmt, va_list args)
{
    char pos_info[MAX_POS_INFO_LEN];

    snprintf(pos_info, MAX_POS_INFO_LEN, "%s:%d", filename, line);
    generic_logger(level, pos_info, fmt, args);
}

#ifndef NDEBUG
void __log_debug(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_with_pos_info(LOG_LEVEL_DEBUG, filename, line, fmt, args);
    va_end(args);
}
#endif

void __log_warning(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_with_pos_info(LOG_LEVEL_WARNING, filename, line, fmt, args);
    va_end(args);
}

void __log_error(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_with_pos_info(LOG_LEVEL_ERROR, filename, line, fmt, args);
    va_end(args);
}

#include <stdlib.h>

void __log_fatal(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_with_pos_info(LOG_LEVEL_FATAL, filename, line, fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

/* ------------------------------------------------------------------------- */

/* all possible combinations of rotating conditions */

static int trigger_size(const struct tm* current, const struct log_tm* old,
                        unsigned long filesize)
{
    return (filesize >= g_max_file_size);
}

static void filename_size(char* buf, int level, const struct tm* ts)
{
    sprintf(buf, "%04d-%02d-%02d_%02d:%02d:%02d.%s",
            ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday,
            ts->tm_hour, ts->tm_min, ts->tm_sec,
            log_level_str_lower[level]);
}

static int trigger_hour(const struct tm* current, const struct log_tm* old,
                        unsigned long filesize)
{
    if ((old->tm_hour == current->tm_hour) &&
        (old->tm_mday == current->tm_mday) &&
        (old->tm_mon == current->tm_mon) &&
        (old->tm_year == current->tm_year))
        return 0;

    return 1;
}

static void filename_hour(char* buf, int level, const struct tm* ts)
{
    sprintf(buf, "%04d-%02d-%02d_%02d.%s",
            ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday, ts->tm_hour,
            log_level_str_lower[level]);
}

static int trigger_day(const struct tm* current, const struct log_tm* old,
                       unsigned long filesize)
{
    if ((old->tm_mday == current->tm_mday) &&
        (old->tm_mon == current->tm_mon) &&
        (old->tm_year == current->tm_year))
        return 0;

    return 1;
}

static void filename_day(char* buf, int level, const struct tm* ts)
{
    sprintf(buf, "%04d-%02d-%02d.%s",
            ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday,
            log_level_str_lower[level]);
}

static int trigger_size_hour(const struct tm* current,
                             const struct log_tm* old, unsigned long filesize)
{
    return (trigger_size(current, old, filesize) ||
            trigger_hour(current, old, filesize));
}

static void filename_size_hour(char* buf, int level, const struct tm* ts)
{
    filename_size(buf, level, ts);
}

static int trigger_size_day(const struct tm* current,
                            const struct log_tm* old, unsigned long filesize)
{
    return (trigger_size(current, old, filesize) ||
            trigger_day(current, old, filesize));
}

static void filename_size_day(char* buf, int level, const struct tm* ts)
{
    filename_size(buf, level, ts);
}

static inline void set_global_functions(unsigned flags)
{
    switch (flags & LOG_ROTATE_FLAG_MASK) {

        case LOG_ROTATE_BY_SIZE:
            g_rotate_trigger = trigger_size;
            g_get_filename = filename_size;
        break;

        case LOG_ROTATE_PER_HOUR:
        case LOG_ROTATE_PER_HOUR | LOG_ROTATE_PER_DAY:
            g_rotate_trigger = trigger_hour;
            g_get_filename = filename_hour;
        break;

        case LOG_ROTATE_PER_DAY:
            g_rotate_trigger = trigger_day;
            g_get_filename = filename_day;
        break;

        case LOG_ROTATE_BY_SIZE | LOG_ROTATE_PER_HOUR:
        case LOG_ROTATE_BY_SIZE | LOG_ROTATE_PER_HOUR | LOG_ROTATE_PER_DAY:
            g_rotate_trigger = trigger_size_hour;
            g_get_filename = filename_size_hour;
        break;

        case LOG_ROTATE_BY_SIZE | LOG_ROTATE_PER_DAY:
            g_rotate_trigger = trigger_size_day;
            g_get_filename = filename_size_day;
        break;

        default:
            g_rotate_trigger = trigger_day;
            g_get_filename = filename_day;
    }
}

/* ------------------------------------------------------------------------- */

static inline void path_init(const char* prefix, int max_prefix_len)
{
    g_path_prefix_len = strlen(prefix);
    if (g_path_prefix_len > max_prefix_len) {
        fprintf(stderr, "prefix len is greater than %d, truncated.\n",
                max_prefix_len);
        g_path_prefix_len = max_prefix_len;
    }

    memcpy(g_path, prefix, g_path_prefix_len);
}

int log_init(const char* prefix, unsigned flags, unsigned int max_megabytes)
{
    int i;

    for (i = 0; i < LOG_LEVEL_MAX; ++i)
        pthread_mutex_init(&g_[i].lock, NULL);

    g_max_file_size = max_megabytes << 20;

    /* set trigger functions according to flags */
    set_global_functions(flags);

    path_init(prefix, MAX_PATH_PREFIX_LEN);

    return 0;
}
