MC = ./tools/clang+llvm-12.0.0-x86_64-apple-darwin/bin/llvm-mc
CC = ./tools/wasi-sdk-12.0/bin/clang

SYSROOT = ./tools/wasi-sdk-12.0/share/wasi-sysroot/
TARGET = wasm32-unknown-wasi

MCFLAGS = -triple=$(TARGET) -filetype=obj -mattr=+exception-handling
CCFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling
LDFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling

LIBOBJS = try_catch.S.o

TESTS = try_catch_test

all: $(LIBOBJS)

test: $(TESTS) run-tests.js
	./run-tests.js $(TESTS)

try_catch_test: try_catch_test.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.S.o: %.S
	$(MC) $(MCFLAGS) $< -o $@

%.c.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

