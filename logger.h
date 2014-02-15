#ifndef __LOGGER_H__
#define __LOGGER_H__

int log_init(const char* prefix);

#define log_info(fmt, ...)      __log_info(fmt, __VA_ARGS__)
#define log_debug(fmt, ...)     __log_debug(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define log_warning(fmt, ...)   __log_warning(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define log_error(fmt, ...)     __log_error(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define log_fatal(fmt, ...)     __log_fatal(__FILE__, __LINE__, fmt, __VA_ARGS__)

void __log_info(const char* fmt, ...);
void __log_debug(const char* filename, int line, const char* fmt, ...);
void __log_warning(const char* filename, int line, const char* fmt, ...);
void __log_error(const char* filename, int line, const char* fmt, ...);
void __log_fatal(const char* filename, int line, const char* fmt, ...);

#endif
