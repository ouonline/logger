#ifndef __LOGGER_H__
#define __LOGGER_H__

/* flag definitions for log_init() */
#define LOG_ROTATE_BY_SIZE      0x1
#define LOG_ROTATE_PER_HOUR     0x2
#define LOG_ROTATE_PER_DAY      0x4

#define LOG_ROTATE_FLAG_MASK    0x7
#define LOG_ROTATE_DEFAULT      LOG_ROTATE_PER_DAY

int log_init(const char* prefix, unsigned flags, unsigned int max_megabytes);

#define log_info(fmt, ...)      __log_info(fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)     __log_debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)   __log_warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     __log_error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)     __log_fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

void __log_info(const char* fmt, ...);
void __log_debug(const char* filename, int line, const char* fmt, ...);
void __log_warning(const char* filename, int line, const char* fmt, ...);
void __log_error(const char* filename, int line, const char* fmt, ...);
void __log_fatal(const char* filename, int line, const char* fmt, ...);

#endif
