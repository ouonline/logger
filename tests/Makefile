CC := gcc

ifeq ($(debug), y)
    CFLAGS := -g
else
    CFLAGS := -O2 -DNDEBUG
endif

CFLAGS := $(CFLAGS) -Wall -Werror

ifndef DEPSDIR
    DEPSDIR := $(shell pwd)/../..
endif

INCLUDE := -I$(DEPSDIR)
LIBS := $(DEPSDIR)/logger/liblogger.a $(DEPSDIR)/utils/libutils.a -lpthread

OBJS := $(patsubst %c, %o, $(wildcard *.c))

TARGET := test_logger

.PHONY: all clean pre-process post-clean

all: $(OBJS) $(TARGET)

pre-process:
	d=$(DEPSDIR)/logger; if ! [ -d $$d ]; then git clone https://github.com/ouonline/logger.git $$d; fi
	$(MAKE) DEPSDIR=$(DEPSDIR) -C $(DEPSDIR)/logger
	$(MAKE) -C ..

post-clean:
	$(MAKE) clean -C ..
	$(MAKE) clean DEPSDIR=$(DEPSDIR) -C $(DEPSDIR)/logger

$(TARGET): $(OBJS) | pre-process
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

clean: | post-clean
	rm -f $(TARGET) $(OBJS)
