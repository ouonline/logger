#include "stdio_logger.h"
#include "utils/utils.h"
#include "utils/time_utils.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_LOG_LEN 4096

static pthread_mutex_t g_stdout_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_stderr_lock = PTHREAD_MUTEX_INITIALIZER;

#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_MISC      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_WARNING   3
#define LOG_LEVEL_ERROR     4
#define LOG_LEVEL_FATAL     5
#define LOG_LEVEL_MAX       6

static const char* log_level_str[] = {
    "DEBUG", "MISC", "\e[1;32mINFO\e[0m", "\e[0;33mWARNING\e[0m", "\e[0;31mERROR\e[0m", "\e[0;31mFATAL\e[0m",
};

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

static void generic_logger(int level, FILE* fp, pthread_mutex_t* lock,
                           const char* filename, int line, const char* funcname,
                           const char* fmt, va_list* args) {
    struct tm tm;
    char timestr[32];
    char buf[MAX_LOG_LEN];

    pthread_mutex_lock(lock);

    current_datetime(timestr, &tm);

    vsnprintf(buf, MAX_LOG_LEN, fmt, *args);
    fprintf(fp, "[%s] [%s] [%s:%u:%s()]\t%s\n",
            log_level_str[level], timestr, filename, line, funcname, buf);
    fflush(fp); /* flush cache to disk */

    pthread_mutex_unlock(lock);
}

static void stdio_logger_debug(struct logger* l, const char* filename, int line,
                               const char* funcname, const char* fmt, ...) {
    (void)l;
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_DEBUG, stdout, &g_stdout_lock, filename,
                   line, funcname, fmt, &args);
    va_end(args);
}

static void stdio_logger_misc(struct logger* l, const char* filename, int line,
                              const char* funcname, const char* fmt, ...) {
    (void)l;
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_MISC, stdout, &g_stdout_lock, filename,
                   line, funcname, fmt, &args);
    va_end(args);
}

static void stdio_logger_info(struct logger* l, const char* filename, int line,
                              const char* funcname, const char* fmt, ...) {
    (void)l;
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_INFO, stdout, &g_stdout_lock, filename,
                   line, funcname, fmt, &args);
    va_end(args);
}

static void stdio_logger_warning(struct logger* l, const char* filename, int line,
                                 const char* funcname, const char* fmt, ...) {
    (void)l;
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_WARNING, stderr, &g_stderr_lock, filename,
                   line, funcname, fmt, &args);
    va_end(args);
}

static void stdio_logger_error(struct logger* l, const char* filename, int line,
                               const char* funcname, const char* fmt, ...) {
    (void)l;
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_ERROR, stderr, &g_stderr_lock, filename,
                   line, funcname, fmt, &args);
    va_end(args);
}

static void stdio_logger_fatal(struct logger* l, const char* filename, int line,
                               const char* funcname, const char* fmt, ...) {
    (void)l;
    va_list args;
    va_start(args, fmt);
    generic_logger(LOG_LEVEL_FATAL, stderr, &g_stderr_lock, filename,
                   line, funcname, fmt, &args);
    va_end(args);
}

static const struct logger_operations stdio_logger_operations = {
    .debug = stdio_logger_debug,
    .misc = stdio_logger_misc,
    .info = stdio_logger_info,
    .warning = stdio_logger_warning,
    .error = stdio_logger_error,
    .fatal = stdio_logger_fatal,
};

void stdio_logger_init(struct logger* l) {
    l->ops = &stdio_logger_operations;
}
