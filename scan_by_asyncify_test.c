#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

// MARK: Asyncify

#define ASYNCIFY_BUF_SIZE 128

struct asyncify_buf {
  void *top;
  void *end;
  uint8_t buffer[ASYNCIFY_BUF_SIZE];
};

void init_asyncify_buf(struct asyncify_buf* buf) {
  buf->top = &buf->buffer[0];
  buf->end = &buf->buffer[ASYNCIFY_BUF_SIZE];
}
__attribute__((import_module("asyncify"), import_name("start_unwind")))
void asyncify_start_unwind(struct asyncify_buf *buf);
__attribute__((import_module("asyncify"), import_name("stop_unwind")))
void asyncify_stop_unwind(void);
__attribute__((import_module("asyncify"), import_name("start_rewind")))
void asyncify_start_rewind(struct asyncify_buf *buf);
__attribute__((import_module("asyncify"), import_name("stop_rewind")))
void asyncify_stop_rewind(void);

// MARK: Helpers for CRuby

typedef void (*rb_wasm_scan_func)(void*, void*);

static struct asyncify_buf *active_asyncify_buf = NULL;

void rb_wasm_scan_locals(rb_wasm_scan_func scan) {
  static struct asyncify_buf buf;
  static int spilling = 0;
  if (!spilling) {
    spilling = 1;
    init_asyncify_buf(&buf);
    active_asyncify_buf = &buf;
    asyncify_start_unwind(&buf);
  } else {
    asyncify_stop_rewind();
    spilling = 0;
    active_asyncify_buf = NULL;
    scan(buf.top, buf.end);
  }
}


void dump_memory(uint8_t *base, uint8_t *end) {
  size_t chunk_size = 16;

  for (uint8_t *ptr = base; ptr <= end; ptr += chunk_size) {
    printf("%8p", ptr);
    for (size_t offset = 0; offset < chunk_size; offset++) {
      printf(" %02x", *(ptr + offset));
    }
    printf("\n");
  }
}


bool find_in_stack(uint32_t target, void *base, void *end) {
  printf("checking %#x ... ", target);

  for (void *ptr = base; ptr < end; ptr += sizeof(uint32_t)) {
    if ((*(uint32_t *)ptr) == target) {
      printf("ok\n");
      return true;
    }
  }
  return false;
}


void please_gc_internal(void *start, void *end) {
  dump_memory(start, end);
  // Find returned value
  assert(find_in_stack(0xdeadbeef, start, end));
}
void please_gc(void) {
  rb_wasm_scan_locals(please_gc_internal);
}

uint32_t v;
__attribute__((noinline))
uint32_t return_value(void) {
  return v;
}

__attribute__((noinline))
uint32_t check_return_value(void) {
  please_gc();
  return 0;
}

__attribute__((noinline))
void take_two_args(uint32_t a, uint32_t b) {
  printf("a + b = %#x\n", a + b);
}

__attribute__((noinline))
int start(void) {
  // to avoid const propagation
  v = 0xdeadbeef;

  printf("before please_gc\n");
  take_two_args(return_value(), check_return_value());
  printf("after please_gc\n");

  return 0;
}

// $ clang -c -target wasm32-unknown-wasi --sysroot /path/to/wasi-sysroot/ -O3 scan_by_asyncify_test.c -o scan_by_asyncify_test.c.o
// $ clang -target wasm32-unknown-wasi --sysroot /path/to/wasi-sysroot/ -g scan_by_asyncify_test.c.o -o scan_by_asyncify_test
// $ wasm-opt -g --asyncify --pass-arg=asyncify-ignore-imports scan_by_asyncify_test -o scan_by_asyncify_test.asyncified
// $ wasmtime scan_by_asyncify_test.asyncified
// before please_gc
// stop the world
//    0xf4c 00 00 00 00 00 00 00 00 01 00 00 00 ef be ad de
//    0xf5c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//    0xf6c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//    0xf7c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//    0xf8c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//    0xf9c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//    0xfac 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//    0xfbc 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// checking 0xdeadbeef ... ok
// a + b = 0xdeadbeef
// after please_gc
int main(void) {
  int result = start();
  while (active_asyncify_buf) {
    asyncify_stop_unwind();
    printf("stop the world\n");
    asyncify_start_rewind(active_asyncify_buf);
    result = start();
  }
  return result;
}

