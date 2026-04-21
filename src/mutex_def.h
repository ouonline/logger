#ifndef __LOGGER_MUTEX_DEF_H__
#define __LOGGER_MUTEX_DEF_H__

#if defined(_WIN32) || defined(_WIN64)

#define NOGDI
#include <windows.h>

typedef CRITICAL_SECTION pthread_mutex_t;

static inline int pthread_mutex_init(pthread_mutex_t* mutex,
                                     void* unused) {
    (void)unused;
    InitializeCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_destroy(pthread_mutex_t* mutex) {
    DeleteCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t* mutex) {
    EnterCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_unlock(pthread_mutex_t* mutex) {
    LeaveCriticalSection(mutex);
    return 0;
}

#else

#include <pthread.h>

#endif

#endif
