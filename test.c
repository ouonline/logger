#include "logger.h"

int main(void)
{
    int a = 5;

    log_init("ouonline.");

    log_debug("a = %d.", a);
    log_debug("a = %d.", a);

    return 0;
}
