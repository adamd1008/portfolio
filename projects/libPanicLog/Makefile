MAJOR_VER=1
MINOR_VER=9
BUILD_VER=0

$(eval UUID := $(shell uuidgen -t))

CC=gcc
CFLAGS=-ansi -c -Wall -Wextra -Werror -rdynamic -fPIC -pedantic \
	-DMAJOR_VER=$(MAJOR_VER) \
	-DMINOR_VER=$(MINOR_VER) \
	-DBUILD_VER=$(BUILD_VER) \
	-DLOG_UUID=\"$(UUID)\"
LIB_LDFLAGS=-shared \
	-Wl,-soname,libPanicLog.so.$(MAJOR_VER).$(MINOR_VER).$(BUILD_VER)
LIB_SOURCES=panic_log.c
LIB_OBJECTS=$(LIB_SOURCES:.c=.o)
LIBRARY=libPanicLog.so.$(MAJOR_VER).$(MINOR_VER).$(BUILD_VER)
LIB_DIR=../../lib

all: $(LIB_SOURCES) $(LIBRARY) $(EXECUTABLE)

$(LIBRARY): $(LIB_OBJECTS) 
	@echo "** Linking file '$<'"
	$(CC) $(LIB_LDFLAGS) $(LIB_OBJECTS) -o $(LIB_DIR)/$@
	ln -sf $@ $(LIB_DIR)/libPanicLog.so

.c.o:
	@echo "** Compiling file '$<'"
	$(CC) $(CFLAGS) $< -o $@

PHONY: clean

clean:
	rm -f $(LIB_DIR)/$(LIBRARY) $(LIB_DIR)/libPanicLog.so $(EXECUTABLE) \
	*.o *~
