#ifndef RB_WASM_SUPPORT_SETJMP_H
#define RB_WASM_SUPPORT_SETJMP_H

#include "rb-wasm-support/config.h"
#include <stdbool.h>

struct __rb_wasm_asyncify_jmp_buf {
  void* top;
  void* end;
  char buffer[RB_WASM_SUPPORT_FRAME_BUFFER_SIZE];
};

typedef struct {
  struct __rb_wasm_asyncify_jmp_buf setjmp_buf;
  struct __rb_wasm_asyncify_jmp_buf longjmp_buf;
  void *dst_buf_top;
  int val;
  int state;
} rb_wasm_jmp_buf;

/// defined in setjmp.S
int _rb_wasm_setjmp(rb_wasm_jmp_buf *env);

void _rb_wasm_longjmp(rb_wasm_jmp_buf *env, int val);

#define rb_wasm_setjmp(env) ((env).state = 0, _rb_wasm_setjmp(&(env)))
#define rb_wasm_longjmp(env, val) _rb_wasm_longjmp(&env, val)

void *rb_wasm_handle_jmp_unwind(void);

#ifdef RB_WASM_SUPPORT_EMULATE_SETJMP

typedef rb_wasm_jmp_buf jmp_buf;

#define setjmp(env) rb_wasm_setjmp(env)
#define longjmp(env, val) rb_wasm_longjmp(env, val)

#endif

#endif
