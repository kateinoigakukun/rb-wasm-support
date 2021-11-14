MC = ./tools/clang+llvm-12.0.0-x86_64-apple-darwin/bin/llvm-mc
CC = ./tools/wasi-sdk-12.0/bin/clang
AR = ./tools/wasi-sdk-12.0/bin/llvm-ar
INSTALL = /usr/bin/install
WASM_OPT = wasm-opt

PREFIX = ./usr

SYSROOT = ./tools/wasi-sdk-12.0/share/wasi-sysroot/
TARGET = wasm32-unknown-wasi

OPTFLAGS =
MCFLAGS = -triple=$(TARGET) -filetype=obj -mattr=+exception-handling
CCFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling -Iinclude $(OPTFLAGS)
LDFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling -Xlinker --stack-first -Xlinker -z -Xlinker stack-size=16777216
ARFLAGS = rcD
ASYNCIFY_FLAGS = -g --pass-arg=asyncify-verbose --pass-arg=asyncify-ignore-imports --pass-arg=asyncify-ignore-indirect

header_dir = ./include/rb-wasm-support
LIBOBJS = machine.S.o machine.c.o setjmp.c.o setjmp.S.o
HEADERS = $(header_dir)/asyncify.h \
	  $(header_dir)/config.h \
	  $(header_dir)/machine.h \
	  $(header_dir)/setjmp.h

LIB_A = librb_wasm_support.a

TESTS = tests/machine_test.asyncified tests/setjmp_test.asyncified

all: $(LIBOBJS) $(LIB_A)
clean:
	rm -rf $(LIB_A) $(LIBOBJS) $(TESTS)

install: all
	mkdir -p $(PREFIX)/lib
	rm -rf $(PREFIX)/lib/$(LIB_A)
	$(INSTALL) $(LIB_A) $(PREFIX)/lib
	rm -rf $(PREFIX)/include/rb-wasm-support
	mkdir -p $(PREFIX)/include/rb-wasm-support
	$(INSTALL) $(HEADERS) $(PREFIX)/include/rb-wasm-support

$(LIB_A): $(LIBOBJS)
	rm -f $(LIB_A)
	$(AR) $(ARFLAGS) $@ $(LIBOBJS)

test: $(TESTS) bin/run-tests.js
	./bin/run-tests.js $(TESTS)

tests/%: tests/%.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

tests/%.asyncified: tests/%
	$(WASM_OPT) --asyncify $(ASYNCIFY_FLAGS) $^ -o $@

%.S.o: %.S
	$(MC) $(MCFLAGS) $< -o $@

%.c.o: %.c $(HEADERS)
	$(CC) -c $(CCFLAGS) $< -o $@

.PHONY: all clean install
