# This Makefile is generated by omake: https://github.com/ouonline/omake.git

CC := gcc
AR := ar

ifeq ($(debug), y)
	CFLAGS := -g
else
	CFLAGS := -O2 -DNDEBUG
endif
CFLAGS := $(CFLAGS) -Wall -Werror -Wextra -fPIC

TARGET := liblogger.a liblogger.so

.PHONY: all clean

all: $(TARGET)

logger_OBJS := ./file_logger.c.logger.o ./stdio_logger.c.logger.o

logger_INCLUDE := -I..

logger_LIBS := ../utils/libutils.a -lpthread

./file_logger.c.logger.o: ./file_logger.c
	$(CC) $(CFLAGS) $(logger_INCLUDE) -c $< -o $@

./stdio_logger.c.logger.o: ./stdio_logger.c
	$(CC) $(CFLAGS) $(logger_INCLUDE) -c $< -o $@

.PHONY: logger_pre_process

$(logger_OBJS): | logger_pre_process

logger_pre_process:
	$(MAKE) debug=$(debug) -C ../utils

liblogger.a: $(logger_OBJS)
	$(AR) rc $@ $^

liblogger.so: $(logger_OBJS)
	$(CC) -shared -o $@ $^ $(logger_LIBS)

clean:
	rm -f $(TARGET) $(logger_OBJS)
