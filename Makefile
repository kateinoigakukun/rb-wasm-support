MC = ./tools/clang+llvm-12.0.0-x86_64-apple-darwin/bin/llvm-mc
CC = ./tools/wasi-sdk-12.0/bin/clang
WASM_OPT = wasm-opt

SYSROOT = ./tools/wasi-sdk-12.0/share/wasi-sysroot/
TARGET = wasm32-unknown-wasi

OPTFLAGS =
MCFLAGS = -triple=$(TARGET) -filetype=obj -mattr=+exception-handling
CCFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling $(OPTFLAGS)
LDFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling
ASYNCIFY_FLAGS = -g --pass-arg=asyncify-verbose --pass-arg=asyncify-ignore-imports

LIBOBJS = try_catch.S.o scan_stack.S.o

TESTS = try_catch_test scan_stack_test scan_by_asyncify_test

all: $(LIBOBJS)
clean:
	rm -rf $(LIBOBJS) $(TESTS) scan_by_asyncify_test.c.o

test: $(TESTS) run-tests.js
	./run-tests.js $(TESTS)

try_catch_test: try_catch_test.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

scan_stack_test: scan_stack_test.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

scan_by_asyncify_test: scan_by_asyncify_test.c.o
	$(CC) $(LDFLAGS) -g $^ -o $@
	$(WASM_OPT) --asyncify $(ASYNCIFY_FLAGS) $@ -o $@

%.S.o: %.S
	$(MC) $(MCFLAGS) $< -o $@

%.c.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

.PHONY: all clean
