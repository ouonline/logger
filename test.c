#include <pthread.h>
#include "global_logger.h"

#define N 10

static void* print(void* arg)
{
    int i, j;

    for (i = 0; i < 999; ++i)
        for (j = 0; j < 9999; ++j)
            log_debug("%s", (const char*)arg);

    return NULL;
}

int main(void)
{
    int i;
    pthread_t pid[N];
    const char* str = "Hello, world!";

    log_init("ouonline.", LOGGER_ROTATE_BY_SIZE | LOGGER_ROTATE_PER_HOUR, 64);

    for (i = 0; i < N; ++i)
        pthread_create(&pid[i], NULL, print, (void*)str);

    for (i = 0; i < N; ++i)
        pthread_join(pid[i], NULL);

    return 0;
}
