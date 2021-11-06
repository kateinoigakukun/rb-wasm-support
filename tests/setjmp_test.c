#include "rb-wasm-support/setjmp.h"
#include "rb-wasm-support/asyncify.h"
#include <stdio.h>
#include <assert.h>

void check_direct(void) {
  jmp_buf buf;
  int val;
  printf("[%s] start\n", __func__);

  rb_wasm_init_jmp_buf(buf);

  printf("[%s] call rb_wasm_setjmp\n", __func__);
  if ((val = rb_wasm_setjmp(buf)) == 0) {
    printf("[%s] rb_wasm_setjmp(buf) == 0\n", __func__);
    printf("[%s] call rb_wasm_longjmp(buf, 2)\n", __func__);
    rb_wasm_longjmp(buf, 2);
    assert(0 && "unreachable after longjmp");
  } else {
    printf("[%s] rb_wasm_setjmp(buf) == %d\n", __func__, val);
    assert(val == 2 && "unexpected returned value");
  }
  printf("[%s] end\n", __func__);
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