MAJOR_VER=1
MINOR_VER=0
BUILD_VER=0

UUID=$(shell uuidgen -r)

CC=gcc
CFLAGS=-c -Wall -Wextra -Werror -rdynamic -fPIC -pedantic -ansi \
	-DMAJOR_VER=$(MAJOR_VER) \
	-DMINOR_VER=$(MINOR_VER) \
	-DBUILD_VER=$(BUILD_VER) -I../common -I../libPanicLog
LIB_SOURCES=panic_lua_cfg.c
LIB_OBJECTS=$(LIB_SOURCES:.c=.o)
LIBRARY=libPanicLuaCfg.so.$(MAJOR_VER).$(MINOR_VER).$(BUILD_VER)
LIB_LDFLAGS=-shared -Wl,-soname,$(LIBRARY)
LIB_DIR=../../lib

all: print_uuid $(LIB_SOURCES) $(LIBRARY)

print_uuid:
	@echo "UUID = '$(UUID)'"

$(LIBRARY): $(LIB_OBJECTS) 
	@echo "** Linking object '$<'"
	$(CC) $(LIB_LDFLAGS) $(LIB_OBJECTS) -o $(LIB_DIR)/$@
	ln -sf $@ $(LIB_DIR)/libPanicLuaCfg.so

.c.o:
	@echo "** Compiling object '$<'"
	$(CC) $(CFLAGS) $< -o $@

PHONY: clean

clean:
	rm -f $(LIB_DIR)/$(LIBRARY) $(LIB_DIR)/libPanicLuaCfg.so $(EXECUTABLE) *.o *~
