#include "logger/logger.h"

static void do_nothing(struct logger* logger, const char* file, int line,
                       unsigned int level, const char* fmt, ...) {
    (void)logger;
    (void)file;
    (void)line;
    (void)level;
    (void)fmt;
}

void dummy_logger_init(struct logger* l) {
    l->func = do_nothing;
}
