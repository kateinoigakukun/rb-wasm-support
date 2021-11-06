#include "rb-wasm-support/setjmp.h"
#include "rb-wasm-support/asyncify.h"
#include <stdio.h>
#include <assert.h>

void check_direct(void) {
  jmp_buf buf;
  int val;
  rb_wasm_init_jmp_buf(buf);

  if ((val = _rb_wasm_setjmp(&buf)) == 0) {
    printf("after setjmp\n");
    rb_wasm_longjmp(buf, 2);
    assert(0 && "unreachable after longjmp");
  } else {
    printf("setjmp returns %d\n", val);
    assert(val == 2 && "unexpected returned value");
  }
}

int start(void) {
  check_direct();
  return 0;
}

int main(void) {
  int result;
  while (1) {
    result = start();
    // NOTE: it's important to call 'asyncify_stop_rewind' here instead in rb_wasm_handle_jmp_unwind
    // because unless that, Asyncify inserts another unwind check here and it unwinds to the root frame.
    asyncify_stop_rewind();
    if (!rb_wasm_handle_jmp_unwind()) {
      break;
    }
  }
  return result;
}