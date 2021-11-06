#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "rb-wasm-support/asyncify.h"
#include "rb-wasm-support/machine.h"
#include "rb-wasm-support/setjmp.h"

#ifdef RB_WASM_ENABLE_DEBUG_LOG
# include <stdio.h>
# define RB_WASM_DEBUG_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
# define RB_WASM_DEBUG_LOG(...)
#endif

enum jmp_buf_state {
  JMP_BUF_STATE_UNINITIALIZED = 0,
  JMP_BUF_STATE_CAPTURING     = 1,
  JMP_BUF_STATE_CAPTURED      = 2,
  JMP_BUF_STATE_RETURNING     = 3,
};

#define NOINLINE __attribute__((noinline))

void async_buf_init(struct __rb_wasm_asyncify_jmp_buf* buf) {
  buf->top = &buf->buffer[0];
  buf->end = &buf->buffer[RB_WASM_SUPPORT_FRAME_BUFFER_SIZE];
}

static jmp_buf *_rb_wasm_active_jmpbuf;

__attribute__((noinline))
int _rb_wasm_setjmp(jmp_buf *env) {
  RB_WASM_DEBUG_LOG("[%s] env = %p, env->state = %d, sp = %p\n", __func__, env, env->state, rb_wasm_get_stack_pointer());
  switch (env->state) {
  case JMP_BUF_STATE_UNINITIALIZED: {
    RB_WASM_DEBUG_LOG("[%s] JMP_BUF_STATE_UNINITIALIZED\n", __func__);
    env->state = JMP_BUF_STATE_CAPTURING;
    env->val = 0;
    env->sp = rb_wasm_get_stack_pointer();
    _rb_wasm_active_jmpbuf = env;
    async_buf_init(&env->setjmp_buf);
    asyncify_start_unwind(&env->setjmp_buf);
    return -1; // return a dummy value
  }
  case JMP_BUF_STATE_CAPTURING: {
    asyncify_stop_rewind();
    RB_WASM_DEBUG_LOG("[%s] JMP_BUF_STATE_CAPTURING\n", __func__);
    env->state = JMP_BUF_STATE_CAPTURED;
    _rb_wasm_active_jmpbuf = NULL;
    return 0;
  }
  case JMP_BUF_STATE_RETURNING: {
    asyncify_stop_rewind();
    RB_WASM_DEBUG_LOG("[%s] JMP_BUF_STATE_RETURNING\n", __func__);
    rb_wasm_set_stack_pointer(env->sp);
    env->state = JMP_BUF_STATE_CAPTURED;
    _rb_wasm_active_jmpbuf = NULL;
    return env->val;
  }
  default:
    assert(0 && "unexpected state");
  }
  return 0;
}

__attribute__((noinline))
void _rb_wasm_longjmp(jmp_buf* env, int value) {
  RB_WASM_DEBUG_LOG("[%s] env = %p, env->state = %d, value = %d\n", __func__, env, env->state, value);
  assert(env->state == JMP_BUF_STATE_CAPTURED);
  assert(value != 0);
  env->state = JMP_BUF_STATE_RETURNING;
  env->val = value;
  _rb_wasm_active_jmpbuf = env;
  async_buf_init(&env->longjmp_buf);
  asyncify_start_unwind(&env->longjmp_buf);
}

bool rb_wasm_handle_jmp_unwind(void) {
  RB_WASM_DEBUG_LOG("[%s] _rb_wasm_active_jmpbuf = %p\n", __func__, _rb_wasm_active_jmpbuf);
  if (!_rb_wasm_active_jmpbuf) {
    return false;
  }

  switch (_rb_wasm_active_jmpbuf->state) {
  case JMP_BUF_STATE_CAPTURING: {
    RB_WASM_DEBUG_LOG("[%s] JMP_BUF_STATE_CAPTURING\n", __func__);
    _rb_wasm_active_jmpbuf->dst_buf_top = _rb_wasm_active_jmpbuf->setjmp_buf.top;
    break;
  }
  case JMP_BUF_STATE_RETURNING: {
    RB_WASM_DEBUG_LOG("[%s] JMP_BUF_STATE_RETURNING\n", __func__);
    _rb_wasm_active_jmpbuf->setjmp_buf.top = _rb_wasm_active_jmpbuf->dst_buf_top;
    break;
  }
  default:
    assert(0 && "unexpected state");
  }
  asyncify_start_rewind(&_rb_wasm_active_jmpbuf->setjmp_buf);
  return true;
}
