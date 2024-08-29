#include "logger/stdio_logger.h"
#include "cutils/utils.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>

#define MAX_LOG_LEN 4096

struct stdio_logger_impl {
    pthread_mutex_t stdout_lock;
    pthread_mutex_t stderr_lock;
};

#ifdef LOGGER_STDOUT_ANSI_COLOR
#define COLOR_LIGHT_BLACK_BEGIN "\e[1;30m"
#define COLOR_LIGHT_RED_BEGIN "\e[1;31m"
#define COLOR_LIGHT_GREEN_BEGIN "\e[1;32m"
#define COLOR_LIGHT_YELLOW_BEGIN "\e[1;33m"
#define COLOR_LIGHT_BLUE_BEGIN "\e[1;34m"
#define COLOR_LIGHT_PURPLE_BEGIN "\e[1;35m"
#define COLOR_LIGHT_CYAN_BEGIN "\e[1;36m"
#define COLOR_LIGHT_WHITE_BEGIN "\e[1;37m"
#define COLOR_END "\e[0m"
#else
#define COLOR_LIGHT_BLACK_BEGIN
#define COLOR_LIGHT_RED_BEGIN
#define COLOR_LIGHT_GREEN_BEGIN
#define COLOR_LIGHT_YELLOW_BEGIN
#define COLOR_LIGHT_BLUE_BEGIN
#define COLOR_LIGHT_PURPLE_BEGIN
#define COLOR_LIGHT_CYAN_BEGIN
#define COLOR_LIGHT_WHITE_BEGIN
#define COLOR_END
#endif

static const char* g_log_level_str[] = {
    COLOR_LIGHT_BLUE_BEGIN"DEBUG"COLOR_END,
    COLOR_LIGHT_GREEN_BEGIN"INFO"COLOR_END,
    COLOR_LIGHT_YELLOW_BEGIN"WARNING"COLOR_END,
    COLOR_LIGHT_RED_BEGIN"ERROR"COLOR_END,
    COLOR_LIGHT_RED_BEGIN"FATAL"COLOR_END,
};

static void generic_logger(int level, FILE* fp, pthread_mutex_t* lock,
                           const char* filename, int line,
                           const char* fmt, va_list* args) {
    struct tm tm;
    char timestr[32];
    char buf[MAX_LOG_LEN];

    pthread_mutex_lock(lock);

    current_datetime(timestr, &tm);

    vsnprintf(buf, MAX_LOG_LEN, fmt, *args);
    fprintf(fp, "[%s] [%s] [%s:%u]\t%s\n",
            g_log_level_str[level], timestr, filename, line, buf);
    fflush(fp); /* flush cache to disk */

    pthread_mutex_unlock(lock);
}

static void stdio_logger_debug(struct logger* l, const char* filename, int line,
                               const char* fmt, ...) {
    if (l->level <= LOGGER_LEVEL_DEBUG) {
        struct stdio_logger* sl = container_of(l, struct stdio_logger, l);
        va_list args;
        va_start(args, fmt);
        generic_logger(LOGGER_LEVEL_DEBUG, stdout, &sl->impl->stdout_lock, filename,
                       line, fmt, &args);
        va_end(args);
    }
}

static void stdio_logger_info(struct logger* l, const char* filename, int line,
                              const char* fmt, ...) {
    if (l->level <= LOGGER_LEVEL_INFO) {
        struct stdio_logger* sl = container_of(l, struct stdio_logger, l);
        va_list args;
        va_start(args, fmt);
        generic_logger(LOGGER_LEVEL_INFO, stdout, &sl->impl->stdout_lock, filename,
                       line, fmt, &args);
        va_end(args);
    }
}

static void stdio_logger_warning(struct logger* l, const char* filename, int line,
                                 const char* fmt, ...) {
    if (l->level <= LOGGER_LEVEL_WARNING) {
        struct stdio_logger* sl = container_of(l, struct stdio_logger, l);
        va_list args;
        va_start(args, fmt);
        generic_logger(LOGGER_LEVEL_WARNING, stderr, &sl->impl->stderr_lock, filename,
                       line, fmt, &args);
        va_end(args);
    }
}

static void stdio_logger_error(struct logger* l, const char* filename, int line,
                               const char* fmt, ...) {
    if (l->level <= LOGGER_LEVEL_ERROR) {
        struct stdio_logger* sl = container_of(l, struct stdio_logger, l);
        va_list args;
        va_start(args, fmt);
        generic_logger(LOGGER_LEVEL_ERROR, stderr, &sl->impl->stderr_lock, filename,
                       line, fmt, &args);
        va_end(args);
    }
}

static void stdio_logger_fatal(struct logger* l, const char* filename, int line,
                               const char* fmt, ...) {
    if (l->level <= LOGGER_LEVEL_FATAL) {
        struct stdio_logger* sl = container_of(l, struct stdio_logger, l);
        va_list args;
        va_start(args, fmt);
        generic_logger(LOGGER_LEVEL_FATAL, stderr, &sl->impl->stderr_lock, filename,
                       line, fmt, &args);
        va_end(args);
    }
}

static const struct logger_operations stdio_logger_operations = {
    .debug = stdio_logger_debug,
    .info = stdio_logger_info,
    .warning = stdio_logger_warning,
    .error = stdio_logger_error,
    .fatal = stdio_logger_fatal,
};

int stdio_logger_init(struct stdio_logger* l) {
    l->impl = malloc(sizeof(struct stdio_logger_impl));
    if (!l->impl) {
        return -1;
    }

    pthread_mutex_init(&l->impl->stdout_lock, NULL);
    pthread_mutex_init(&l->impl->stderr_lock, NULL);

    l->l.level = LOGGER_LEVEL_DEBUG;
    l->l.ops = &stdio_logger_operations;

    return 0;
}

void stdio_logger_destroy(struct stdio_logger* l) {
    if (l->impl) {
        pthread_mutex_destroy(&l->impl->stdout_lock);
        pthread_mutex_destroy(&l->impl->stderr_lock);
        free(l->impl);
        l->impl = NULL;
    }
}
