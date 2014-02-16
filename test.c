#include "logger.h"

int main(void)
{
    const char* str = "Hello, world!";
    log_init("ouonline.", LOG_ROTATE_BY_SIZE, 0);

    log_debug("str -> %s.", str);
    log_info("str -> %s.", str);
    log_warning("str -> %s.", str);
    log_error("str -> %s.", str);
    log_fatal("str -> %s.", str);

    return 0;
}
