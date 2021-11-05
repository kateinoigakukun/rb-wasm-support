MC = ./tools/clang+llvm-12.0.0-x86_64-apple-darwin/bin/llvm-mc
CC = ./tools/wasi-sdk-12.0/bin/clang
WASM_OPT = wasm-opt

SYSROOT = ./tools/wasi-sdk-12.0/share/wasi-sysroot/
TARGET = wasm32-unknown-wasi

OPTFLAGS =
MCFLAGS = -triple=$(TARGET) -filetype=obj -mattr=+exception-handling
CCFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling -Iinclude $(OPTFLAGS)
LDFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling
ASYNCIFY_FLAGS = -g --pass-arg=asyncify-verbose --pass-arg=asyncify-ignore-imports

LIBOBJS = try_catch.S.o machine.S.o machine.c.o

TESTS = try_catch_test machine_test

all: $(LIBOBJS)
clean:
	rm -rf $(LIBOBJS) $(TESTS) scan_by_asyncify_test.c.o machine_test.c.o

test: $(TESTS) run-tests.js
	./run-tests.js $(TESTS)

try_catch_test: try_catch_test.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

machine_test: machine_test.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(WASM_OPT) --asyncify $(ASYNCIFY_FLAGS) $@ -o $@

%.S.o: %.S
	$(MC) $(MCFLAGS) $< -o $@

%.c.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

.PHONY: all clean
