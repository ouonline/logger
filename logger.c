#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "../mm/mm.h"
#include "logger.h"

/* ------------------------------------------------------------------------- */

#define LEVEL_DEBUG     0
#define LEVEL_INFO      1
#define LEVEL_WARNING   2
#define LEVEL_ERROR     3
#define LEVEL_FATAL     4
#define LEVEL_MAX       5

static const char* level_str_upper[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL",
};

static const char* level_str_lower[] = {
    "debug", "info", "warning", "error", "fatal",
};

static struct {
    FILE* fp;
    pthread_mutex_t lock;
} g_[LEVEL_MAX];

/* log filename will be <prefix>YYYY-MM-DD.<level>
 * max(strlen(level_str[])) == strlen("warning") == 7 */
#define PATH_BUFLEN             1024
#define MAX_PATH_LEN            (PATH_BUFLEN - 1)
#define MAX_PATH_PREFIX_LEN     (MAX_PATH_LEN - 18)

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

static inline void new_log_level_file(int level)
{
    char timestr[32];
    current_date(timestr, 32);

    pthread_mutex_lock(&g_[level].lock);

    if (g_[level].fp && g_[level].fp != stdout && g_[level].fp != stderr)
        fclose(g_[level].fp);

    sprintf(g_path + g_path_prefix_len, "%s.%s",
            timestr, level_str_lower[level]);

    g_[level].fp = fopen(g_path, "a");
    if (!g_[level].fp) {
        fprintf(stderr, "cannot create/open file `%s', "
                "log content of %s is redirected to %s.\n",
                g_path, level_str_upper[level],
                (level == LEVEL_INFO) ? "stdout" : "stderr");
        g_[level].fp = (level == LEVEL_INFO) ? stdout : stderr;
    }

    pthread_mutex_unlock(&g_[level].lock);
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

static inline void generic_logger(int level, const char* filename, int line,
                                  const char* fmt, va_list args)
{
    struct tm tm;
    char timestr[32];

    current_datetime(timestr, 32, &tm);

    if (is_another_day(&tm))
        new_log_level_file(level);

    pthread_mutex_lock(&g_[level].lock);
    fprintf(g_[level].fp, "%s %s %s:%d\t", timestr,
            level_str_upper[level], filename, line);
    vfprintf(g_[level].fp, fmt, args);
    pthread_mutex_unlock(&g_[level].lock);
}

void __log_info(const char* fmt, ...)
{
    va_list args;
    struct tm tm;
    char timestr[32];

    current_datetime(timestr, 32, &tm);

    if (is_another_day(&tm))
        new_log_level_file(LEVEL_INFO);

    va_start(args, fmt);

    pthread_mutex_lock(&g_[LEVEL_INFO].lock);
    fprintf(g_[LEVEL_INFO].fp, "%s %s\t", timestr,
            level_str_upper[LEVEL_INFO]);
    vfprintf(g_[LEVEL_INFO].fp, fmt, args);
    pthread_mutex_unlock(&g_[LEVEL_INFO].lock);

    va_end(args);
}

void __log_debug(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    generic_logger(LEVEL_DEBUG, filename, line, fmt, args);
    va_end(args);
}

void __log_warning(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    generic_logger(LEVEL_WARNING, filename, line, fmt, args);
    va_end(args);
}

void __log_error(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    generic_logger(LEVEL_ERROR, filename, line, fmt, args);
    va_end(args);
}

#include <stdlib.h>

void __log_fatal(const char* filename, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    generic_logger(LEVEL_FATAL, filename, line, fmt, args);
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

    for (i = 0; i < LEVEL_MAX; ++i)
        pthread_mutex_init(&g_[i].lock, NULL);

    for (i = 0; i < LEVEL_MAX; ++i)
        new_log_level_file(i);

    return 0;
}
