#include <stdlib.h>
#include "rb-wasm-support/fiber.h"
#include "rb-wasm-support/asyncify.h"

#ifdef RB_WASM_ENABLE_DEBUG_LOG
# include <stdio.h>
# define RB_WASM_DEBUG_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
# define RB_WASM_DEBUG_LOG(...)
#endif

void rb_wasm_init_context(rb_wasm_fiber_context *fcp) {
  fcp->asyncify_buf.top = &fcp->asyncify_buf.buffer[0];
  fcp->asyncify_buf.end = &fcp->asyncify_buf.buffer[RB_WASM_SUPPORT_FRAME_BUFFER_SIZE];
  fcp->is_rewinding = false;
  fcp->is_started = false;
  RB_WASM_DEBUG_LOG("[%s] fcp->asyncify_buf %p\n", __func__, &fcp->asyncify_buf);
}

void rb_wasm_makecontext(rb_wasm_fiber_context *fcp, void (*func)(void *, void *), void *arg0, void *arg1) {
  fcp->entry_point = func;
  fcp->arg0 = arg0;
  fcp->arg1 = arg1;
}

static rb_wasm_fiber_context *_rb_wasm_active_next_fiber;

void rb_wasm_swapcontext(rb_wasm_fiber_context *ofcp, rb_wasm_fiber_context *fcp) {
  RB_WASM_DEBUG_LOG("[%s] enter ofcp = %p fcp = %p\n", __func__, ofcp, fcp);
  if (ofcp->is_rewinding) {
    asyncify_stop_rewind();
    ofcp->is_rewinding = false;
    return;
  }
  _rb_wasm_active_next_fiber = fcp;
  RB_WASM_DEBUG_LOG("[%s] start unwinding asyncify_buf = %p\n", __func__, &ofcp->asyncify_buf);
  asyncify_start_unwind(&ofcp->asyncify_buf);
}

void *rb_wasm_handle_fiber_unwind(void (**new_fiber_entry)(void *, void *), 
                                  void **arg0, void **arg1) {
  rb_wasm_fiber_context *next_fiber;
  if (!_rb_wasm_active_next_fiber) {
    *new_fiber_entry = NULL;
    RB_WASM_DEBUG_LOG("[%s] no next fiber\n", __func__);
    return NULL;
  }

  next_fiber = _rb_wasm_active_next_fiber;
  _rb_wasm_active_next_fiber = NULL;

  RB_WASM_DEBUG_LOG("[%s] next_fiber->asyncify_buf = %p\n", __func__, &next_fiber->asyncify_buf);

  *new_fiber_entry = next_fiber->entry_point;
  *arg0 = next_fiber->arg0;
  *arg1 = next_fiber->arg1;
  if (!next_fiber->is_started) {
    RB_WASM_DEBUG_LOG("[%s] new fiber started\n", __func__);
    // start a new fiber if not started yet
    next_fiber->is_started = true;
    return NULL;
  } else {
    RB_WASM_DEBUG_LOG("[%s] resume a fiber\n", __func__);
    // resume a fiber again
    next_fiber->is_rewinding = true;
    return &next_fiber->asyncify_buf;
  }
}
