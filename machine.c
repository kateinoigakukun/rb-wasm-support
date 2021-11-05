#include <stdlib.h>
#include "rb-wasm-support/machine.h"
#include "rb-wasm-support/asyncify.h"

#define ASYNCIFY_BUF_SIZE 2048

struct asyncify_buf {
  void *top;
  void *end;
  uint8_t buffer[ASYNCIFY_BUF_SIZE];
};

void init_asyncify_buf(struct asyncify_buf* buf) {
  buf->top = &buf->buffer[0];
  buf->end = &buf->buffer[ASYNCIFY_BUF_SIZE];
}

void rb_wasm_scan_locals(rb_wasm_scan_func scan) {
  static struct asyncify_buf buf;
  static int spilling = 0;
  if (!spilling) {
    spilling = 1;
    init_asyncify_buf(&buf);
    rb_asyncify_set_active_buf(&buf);
    asyncify_start_unwind(&buf);
  } else {
    asyncify_stop_rewind();
    spilling = 0;
    rb_asyncify_set_active_buf(NULL);
    scan(buf.top, buf.end);
  }
}

static void *rb_wasm_stack_base = NULL;

int __attribute__((constructor)) rb_wasm_record_stack_base(void) {
  rb_wasm_stack_base = rb_wasm_get_stack_pointer();
  return 0;
}

void _rb_wasm_scan_stack(rb_wasm_scan_func scan, void *current) {
  scan(current, rb_wasm_stack_base);
}

static void *_rb_asyncify_active_buf = NULL;
void rb_asyncify_set_active_buf(void *buf) {
  _rb_asyncify_active_buf = buf;
}
void *rb_asyncify_get_active_buf(void) {
  return _rb_asyncify_active_buf;
}
