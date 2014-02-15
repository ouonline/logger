#include "logger.h"

int main(void)
{
    int a = 5;

    log_init("ouonline.");

    log_debug("a = %d.\n", a);
    log_debug("a = %d.\n", a);

    return 0;
}
