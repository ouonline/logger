#include "stdio_logger.h"
#include "utils/utils.h"
#include "utils/time_utils.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_LOG_LEN 1024

static pthread_mutex_t g_stdout_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_stderr_lock = PTHREAD_MUTEX_INITIALIZER;

static void generic_logger(FILE* fp, pthread_mutex_t* lock,
                           const char* filename, int line,
                           const char* fmt, va_list* args) {
    struct tm tm;
    char timestr[32];
    char buf[MAX_LOG_LEN];

    pthread_mutex_lock(lock);

    current_datetime(timestr, &tm);

    vsnprintf(buf, MAX_LOG_LEN, fmt, *args);
    fprintf(fp, "[%s] [%s:%u]\t%s\n", timestr, filename, line, buf);
    fflush(fp); /* flush cache to disk */

    pthread_mutex_unlock(lock);
}

static void logger_stdout(struct logger* l, const char* filename, int line,
                          const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    generic_logger(stdout, &g_stdout_lock, filename, line, fmt, &args);
    va_end(args);
}

static void logger_stderr(struct logger* l, const char* filename, int line,
                          const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    generic_logger(stderr, &g_stderr_lock, filename, line, fmt, &args);
    va_end(args);
}

static const struct logger_operations stdio_logger_operations = {
    .debug = logger_stdout,
    .misc = logger_stdout,
    .info = logger_stdout,
    .warning = logger_stderr,
    .error = logger_stderr,
    .fatal = logger_stderr,
};

void stdio_logger_init(struct logger* l) {
    l->ops = &stdio_logger_operations;
}
