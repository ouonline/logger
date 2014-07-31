#include <stdarg.h>
#include "logger.h"

/* external functions implemented in logger.c */

#ifndef NDEBUG
extern void __vlogger_debug(struct logger*, const char* filename, int line,
                            const char* fmt, va_list args);
#endif

extern void __vlogger_user(struct logger*, const char* filename, int line,
                           const char* fmt, va_list args);
extern void __vlogger_info(struct logger*, const char* filename, int line,
                           const char* fmt, va_list args);
extern void __vlogger_warning(struct logger*, const char* filename, int line,
                              const char* fmt, va_list args);
extern void __vlogger_error(struct logger*, const char* filename, int line,
                            const char* fmt, va_list args);
extern void __vlogger_fatal(struct logger*, const char* filename, int line,
                            const char* fmt, va_list args);

/* ------------------------------------------------------------------------- */

static struct logger o_o;

int log_init(const char* prefix, unsigned int flags,
             unsigned int max_megabytes)
{
    return logger_init(&o_o, prefix, flags, max_megabytes);
}

void log_destroy(void)
{
    logger_destroy(&o_o);
}

#ifndef NDEBUG
void __log_debug(const char* filename, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    __vlogger_debug(&o_o, filename, line, fmt, args);
    va_end(args);
}
#endif

void __log_user(const char* filename, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    __vlogger_user(&o_o, filename, line, fmt, args);
    va_end(args);
}

void __log_info(const char* filename, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    __vlogger_info(&o_o, filename, line, fmt, args);
    va_end(args);
}

void __log_warning(const char* filename, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    __vlogger_warning(&o_o, filename, line, fmt, args);
    va_end(args);
}

void __log_error(const char* filename, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    __vlogger_error(&o_o, filename, line, fmt, args);
    va_end(args);
}

void __log_fatal(const char* filename, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    __vlogger_fatal(&o_o, filename, line, fmt, args);
    va_end(args);
}
