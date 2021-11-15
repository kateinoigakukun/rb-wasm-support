#include "rb-wasm-support/fiber.h"
#include "rb-wasm-support/asyncify.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static rb_wasm_fiber_context fctx_main, fctx_func1, fctx_func2;

static int counter = 0;

static void func1(void *arg0, void *arg1) {
  assert(counter == 2);
  fprintf(stderr, "func1: started\n");
  fprintf(stderr, "func1: swapcontext(&fctx_func1, &fctx_func2)\n");
  counter++;
  rb_wasm_swapcontext(&fctx_func1, &fctx_func2);

  fprintf(stderr, "func1: returning\n");
}

static void func2(void *arg0, void *arg1) {
  assert(counter == 1);
  fprintf(stderr, "func2: started\n");
  fprintf(stderr, "func2: swapcontext(&fctx_func2, &fctx_func1)\n");
  counter++;
  rb_wasm_swapcontext(&fctx_func2, &fctx_func1);

  assert(counter == 3);
  fprintf(stderr, "func2: swapcontext(&fctx_func2, &fctx_main)\n");
  counter++;
  rb_wasm_swapcontext(&fctx_func2, &fctx_main);

  fprintf(stderr, "func2: returning\n");
}

// top level function should not be inlined to stop unwinding immediately after this function returns
__attribute__((noinline))
int start() {
  rb_wasm_init_context(&fctx_main);
  fctx_main.is_started = true;

  rb_wasm_init_context(&fctx_func1);
  rb_wasm_makecontext(&fctx_func1, func1, 0, 0);

  rb_wasm_init_context(&fctx_func2);
  rb_wasm_makecontext(&fctx_func2, func2, 0, 0);

  counter++;
  fprintf(stderr, "start: swapcontext(&uctx_main, &fctx_func2)\n");
  rb_wasm_swapcontext(&fctx_main, &fctx_func2);
  assert(counter == 4);

  fprintf(stderr, "start: exiting\n");
  return 42;
}

int main(void) {
  void *buf;
  bool new_fiber_started;
  void (*fiber_entry_point)(void *, void *) = NULL;
  void (*old_entry_point)(void *, void *);
  void *arg0 = NULL, *arg1 = NULL;
  int result;

  while (1) {
    if (fiber_entry_point) {
      fiber_entry_point(arg0, arg1);
    } else {
      result = start();
    }
    // NOTE: it's important to call 'asyncify_stop_unwind' here instead in rb_wasm_handle_jmp_unwind
    // because unless that, Asyncify inserts another unwind check here and it unwinds to the root frame.
    asyncify_stop_unwind();
    old_entry_point = fiber_entry_point;
    buf = rb_wasm_handle_fiber_unwind(&fiber_entry_point, &arg0, &arg1);
    if (buf) {
      fprintf(stderr, "asyncify_start_rewind\n");
      asyncify_start_rewind(buf);
      continue;
    } else if (old_entry_point != fiber_entry_point) {
      fprintf(stderr, "new_fiber_started\n");
      continue;
    }
    break;
  }
  assert(result == 42);
  fprintf(stderr, "main: exiting\n");
  return 0;
}
