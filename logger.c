#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "logger.h"

/* ------------------------------------------------------------------------- */

#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_ERROR     3
#define LOG_LEVEL_FATAL     4
#define LOG_LEVEL_MAX       5

static const char* log_level_str_lower[] = {
    "debug", "info", "warning", "error", "fatal",
};

#define MAX_LOG_LEN (4096 - sizeof(FILE*) - sizeof(pthread_mutex_t))

static struct {
    FILE* fp;
    pthread_mutex_t lock;
    char buf[MAX_LOG_LEN];
} __attribute__((packed)) g_[LOG_LEVEL_MAX];

/* log filename will be <prefix>YYYY-MM-DD.<level>
 * max(strlen(level_str[])) == strlen("warning") == 7 */
#define PATH_BUFLEN             1024
#define MAX_PATH_LEN            (PATH_BUFLEN - 1)
#define MAX_PATH_PREFIX_LEN     (MAX_PATH_LEN - 19 /* 19 >= YYYY-MM-DD.<level> */)

static int g_path_prefix_len;
static char g_path[PATH_BUFLEN];

/* ------------------------------------------------------------------------- */

/* YYYY-MM-DD */
static inline void current_date(char *buf, int size)
{
    time_t t;
    struct tm tm;

    time(&t);
    localtime_r(&t, &tm);
    strftime(buf, size, "%F", &tm);
}

static inline void __new_log_level_file(int level)
{
    char timestr[32];
    current_date(timestr, 32);

    if (g_[level].fp && g_[level].fp != stdout && g_[level].fp != stderr)
        fclose(g_[level].fp);

    sprintf(g_path + g_path_prefix_len, "%s.%s",
            timestr, log_level_str_lower[level]);

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

static inline int is_another_day(struct tm* tp)
{
    static unsigned int day = 0, mon = 0, year = 0;

    if (tp->tm_mday == day && tp->tm_mon == mon && tp->tm_year == year)
        return 0;

    day = tp->tm_mday;
    mon = tp->tm_mon;
    year = tp->tm_year;

    return 1;
}

static inline void generic_logger(int level, const char* extra_info,
                                  const char* fmt, va_list args)
{
    int len;
    struct tm tm;
    char timestr[32];

    current_datetime(timestr, 32, &tm);

    pthread_mutex_lock(&g_[level].lock);

    if (is_another_day(&tm))
        __new_log_level_file(level);

    len = sprintf(g_[level].buf, "%s %s\t", timestr, extra_info);
    vsnprintf(g_[level].buf + len, MAX_LOG_LEN - len, fmt, args);
    fprintf(g_[level].fp, "%s\n", g_[level].buf);

    pthread_mutex_unlock(&g_[level].lock);
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

void __log_debug(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_with_pos_info(LOG_LEVEL_DEBUG, filename, line, fmt, args);
    va_end(args);
}

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

static inline void __date_init(void)
{
    time_t t;
    struct tm tm;

    time(&t);
    localtime_r(&t, &tm);
    is_another_day(&tm);
}

static inline void path_buffer_init(const char* prefix)
{
    g_path_prefix_len = strlen(prefix);
    if (g_path_prefix_len > MAX_PATH_PREFIX_LEN) {
        fprintf(stderr, "prefix len is greater than %d, truncated.\n",
                MAX_PATH_PREFIX_LEN);
        g_path_prefix_len = MAX_PATH_PREFIX_LEN;
    }

    memcpy(g_path, prefix, g_path_prefix_len);
}

int log_init(const char* prefix)
{
    int i;

    __date_init();

    path_buffer_init(prefix);

    for (i = 0; i < LOG_LEVEL_MAX; ++i)
        pthread_mutex_init(&g_[i].lock, NULL);

    for (i = 0; i < LOG_LEVEL_MAX; ++i)
        __new_log_level_file(i);

    return 0;
}
