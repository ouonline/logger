#ifndef __LOGGER_UTILS_H__
#define __LOGGER_UTILS_H__

#include <time.h>
#include <stdio.h>
#include <stddef.h> // offsetof
#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif
#else
#include <sys/time.h>
#endif

static inline void current_datetime(char buf[], struct tm* tp) {
    struct tm ltm;
    if (!tp) {
        tp = &ltm;
    }

#if defined(_WIN32) || defined(_WIN64)
    SYSTEMTIME st;
    GetLocalTime(&st);
    tp->tm_year = st.wYear - 1900;
    tp->tm_mon = st.wMonth - 1;
    tp->tm_mday = st.wDay;
    tp->tm_hour = st.wHour;
    tp->tm_min = st.wMinute;
    tp->tm_sec = st.wSecond;
    tp->tm_isdst = -1;
    snprintf(buf, 27, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth,
             st.wDay, st.wHour + 8, st.wMinute, st.wSecond, st.wMilliseconds);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, tp);
    snprintf(buf, 27, "%04d-%02d-%02d %02d:%02d:%02d.%06d", tp->tm_year + 1900, tp->tm_mon + 1,
             tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec, (int)tv.tv_usec);
#endif
}

#define container_of(ptr, type, member)                     \
    ((type*)((uintptr_t)(ptr) - offsetof(type, member)))

#endif
