CC := gcc
AR := ar

ifeq ($(debug), y)
    CFLAGS := -g
else
    CFLAGS := -O2 -DNDEBUG
endif
CFLAGS := $(CFLAGS) -Wall -Werror -fPIC

ifndef DEPSDIR
    DEPSDIR := $(shell pwd)/..
endif

MODULE_NAME := logger

INCLUDE := -I$(DEPSDIR)
LIBS := -lpthread

OBJS := $(patsubst %.c, %.o, $(wildcard *.c))

TARGET := lib$(MODULE_NAME).a lib$(MODULE_NAME).so

.PHONY: all clean pre-process post-clean

all: $(TARGET)

pre-process:
	d=$(DEPSDIR)/utils; if ! [ -d $$d ]; then git clone https://github.com/ouonline/utils.git $$d; fi
	$(MAKE) DEPSDIR=$(DEPSDIR) -C $(DEPSDIR)/utils

post-clean:
	$(MAKE) DEPSDIR=$(DEPSDIR) clean -C $(DEPSDIR)/utils

lib$(MODULE_NAME).a: $(OBJS) | pre-process
	$(AR) rc $@ $^

lib$(MODULE_NAME).so: $(OBJS) | pre-process
	$(CC) -shared -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean: | post-clean
	rm -f $(TARGET) $(OBJS)
