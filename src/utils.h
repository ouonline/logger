#ifndef __LOGGER_UTILS_H__
#define __LOGGER_UTILS_H__

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

static inline void current_datetime(char buf[], struct tm* tp) {
    struct tm ltm;
    if (!tp) {
        tp = &ltm;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, tp);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%06d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec, tv.tv_usec);
}

#endif
