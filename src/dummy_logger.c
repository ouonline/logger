#include "logger/logger.h"

static void do_nothing(struct logger*, const char*, int, const char*, ...) {}

static const struct logger_operations g_ops = {
    .debug = do_nothing,
    .info = do_nothing,
    .warning = do_nothing,
    .error = do_nothing,
    .fatal = do_nothing,
};

void dummy_logger_init(struct logger* l) {
    l->ops = &g_ops;
}
