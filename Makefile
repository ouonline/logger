CC := gcc

AR := ar
ARFLAGS := rs

INSTALL_PREFIX := /usr/local
INSTALL_LIB := ${INSTALL_PREFIX}/share/liblogger
INSTALL_SO := /usr/lib
INSTALL_INC := /usr/include/liblogger

ifeq ($(debug), y)
    CFLAGS := -g
else
    CFLAGS := -O2 -DNDEBUG
endif

CFLAGS := $(CFLAGS) -Wall -Werror -fPIC

LIBS := -lpthread

OBJS := $(patsubst %c, %o, $(wildcard *.c))

TARGET := test_logger

.PHONY: all clean

all: $(OBJS) $(TARGET) liblogger.so.1

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

liblogger.so.1: logger.o
	$(CC) $(CFLAGS) $^ -shared -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) liblogger.so.1

install: all
	@[ -d ${INSTALL_LIB} ] || mkdir ${INSTALL_LIB}
	@[ -d ${INSTALL_INC} ] || mkdir ${INSTALL_INC}
	cp ./*.h ${INSTALL_INC}
	cp ./liblogger.pc /usr/share/pkgconfig
	cp ./*.so.* ${INSTALL_LIB}
	ln -s ${INSTALL_LIB}/liblogger.so.* ${INSTALL_SO}/liblogger.so
	@echo "Finished !"
	
