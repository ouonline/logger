#include <pthread.h>
#include "file_logger.h"

#define N 10

static void* print(void* arg)
{
    int i, j;
    struct file_logger* fl = (struct file_logger*)arg;

    for (i = 0; i < 999; ++i)
        for (j = 0; j < 9999; ++j) {
            logger_debug(&fl->l, "hello");
            logger_warning(&fl->l, "hello");
        }

    return NULL;
}

int main(void)
{
    int i;
    pthread_t pid[N];
    struct file_logger fl;

    file_logger_init(&fl, ".", "ouonline", LOGGER_ROTATE_BY_SIZE | LOGGER_ROTATE_PER_HOUR, 64);

    for (i = 0; i < N; ++i) {
        pthread_create(&pid[i], NULL, print, (void*)(&fl));
    }

    for (i = 0; i < N; ++i) {
        pthread_join(pid[i], NULL);
    }

    file_logger_destroy(&fl);

    return 0;
}
