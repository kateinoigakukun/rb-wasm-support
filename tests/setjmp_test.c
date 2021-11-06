#include "rb-wasm-support/setjmp.h"
#include "rb-wasm-support/asyncify.h"
#include "rb-wasm-support/machine.h"
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
    printf("[%s] sp = %p\n", __func__, rb_wasm_get_stack_pointer());
    assert(val == 2 && "unexpected returned value");
  }
  printf("[%s] end\n", __func__);
}

void jump_to_dst(jmp_buf *dst) {
  jmp_buf buf;
  printf("[%s] start sp = %p\n", __func__, rb_wasm_get_stack_pointer());

  rb_wasm_init_jmp_buf(buf);

  printf("[%s] call rb_wasm_setjmp\n", __func__);
  if (rb_wasm_setjmp(buf) == 0) {
    printf("[%s] rb_wasm_setjmp(buf) == 0\n", __func__);
    printf("[%s] call rb_wasm_longjmp(dst, 4)\n", __func__);
    rb_wasm_longjmp(*dst, 4);
    assert(0 && "unreachable after longjmp");
  } else {
    assert(0 && "unreachable");
  }
  printf("[%s] end\n", __func__);
}

void check_jump_two_level(void) {
  jmp_buf buf;
  int val;
  printf("[%s] start\n", __func__);

  rb_wasm_init_jmp_buf(buf);

  printf("[%s] call rb_wasm_setjmp\n", __func__);
  if ((val = rb_wasm_setjmp(buf)) == 0) {
    printf("[%s] rb_wasm_setjmp(buf) == 0\n", __func__);
    printf("[%s] call jump_to_dst(&buf)\n", __func__);
    jump_to_dst(&buf);
    assert(0 && "unreachable after longjmp");
  } else {
    printf("[%s] rb_wasm_setjmp(buf) == %d\n", __func__, val);
    assert(val == 4 && "unexpected returned value");
  }
  printf("[%s] end\n", __func__);
}

void check_reuse(void) {
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
    if (val < 5) {
      printf("[%s] re-call rb_wasm_longjmp(buf, %d)\n", __func__, val + 1);
      rb_wasm_longjmp(buf, val + 1);
    }
  }
  printf("[%s] end\n", __func__);
}

void check_stack_ptr(void) {
  static void *normal_sp;
  int val;
  jmp_buf buf;
  volatile int load_from_stack;

  normal_sp = rb_wasm_get_stack_pointer();
  printf("[%s] start sp = %p\n", __func__, normal_sp);

  rb_wasm_init_jmp_buf(buf);

  printf("[%s] call rb_wasm_setjmp\n", __func__);
  if ((val = rb_wasm_setjmp(buf)) == 0) {
    printf("[%s] call jump_to_dst(&buf)\n", __func__);
    jump_to_dst(&buf);
    assert(0 && "unreachable after longjmp");
  } else {
    printf("[%s] rb_wasm_setjmp(buf) == %d\n", __func__, val);
    printf("[%s] sp = %p\n", __func__, rb_wasm_get_stack_pointer());
    assert(rb_wasm_get_stack_pointer() == normal_sp);
  }
  printf("[%s] end\n", __func__);
}

// top level function should not be inlined to stop unwinding immediately after this function returns
__attribute__((noinline))
int start(void) {
  check_direct();
  check_jump_two_level();
  check_reuse();
  check_stack_ptr();
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