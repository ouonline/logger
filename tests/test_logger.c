#include "logger/file_logger.h"

int main(void)
{
    int i, j;
    struct file_logger fl;

    file_logger_init(&fl, ".", "ouonline", LOGGER_ROTATE_BY_SIZE | LOGGER_ROTATE_PER_HOUR, 64);

    for (i = 0; i < 9; ++i) {
        for (j = 0; j < 9; ++j) {
            logger_debug(&fl.l, "debug");
            logger_info(&fl.l, "info");
            logger_warn(&fl.l, "warn");
        }
    }

    file_logger_destroy(&fl);

    return 0;
}
