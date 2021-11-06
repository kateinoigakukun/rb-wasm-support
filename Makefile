MC = ./tools/clang+llvm-12.0.0-x86_64-apple-darwin/bin/llvm-mc
CC = ./tools/wasi-sdk-12.0/bin/clang
WASM_OPT = wasm-opt

SYSROOT = ./tools/wasi-sdk-12.0/share/wasi-sysroot/
TARGET = wasm32-unknown-wasi

OPTFLAGS =
MCFLAGS = -triple=$(TARGET) -filetype=obj -mattr=+exception-handling
CCFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling -Iinclude $(OPTFLAGS)
LDFLAGS = -target $(TARGET) --sysroot $(SYSROOT) -mexception-handling -Xlinker --stack-first -Xlinker -z -Xlinker stack-size=16777216
ASYNCIFY_FLAGS = -g --pass-arg=asyncify-verbose --pass-arg=asyncify-ignore-imports --pass-arg=asyncify-ignore-indirect

LIBOBJS = try_catch.S.o machine.S.o machine.c.o setjmp.c.o

TESTS = tests/try_catch_test tests/machine_test.asyncified tests/setjmp_test.asyncified

all: $(LIBOBJS)
clean:
	rm -rf $(LIBOBJS) $(TESTS) tests/try_catch_test.c.o tests/machine_test.c.o tests/setjmp_test.c.o

test: $(TESTS) bin/run-tests.js
	./bin/run-tests.js $(TESTS)

tests/%: tests/%.c.o $(LIBOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

tests/%.asyncified: tests/%
	$(WASM_OPT) --asyncify $(ASYNCIFY_FLAGS) $^ -o $@

%.S.o: %.S
	$(MC) $(MCFLAGS) $< -o $@

%.c.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

.PHONY: all clean
