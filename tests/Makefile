CC := gcc

ifeq ($(debug), y)
    CFLAGS := -g
else
    CFLAGS := -O2 -DNDEBUG
endif

CFLAGS := $(CFLAGS) -Wall -Werror

INCLUDE := -I..
LIBS := ../liblogger.a -lpthread

OBJS := $(patsubst %c, %o, $(wildcard *.c))

TARGET := test_logger

.PHONY: all clean pre-process post-clean

all: $(OBJS) $(TARGET)

pre-process:
	$(MAKE) -C ..

post-clean:
	$(MAKE) clean -C ..

$(TARGET): $(OBJS) | pre-process
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

clean: | post-clean
	rm -f $(TARGET) $(OBJS)
