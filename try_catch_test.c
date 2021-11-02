#include <stdio.h>
#include <assert.h>

__attribute__((noreturn))
void rb_wasm_throw(int val);
/// returns 0 if succeed, otherwise returns a thrown value
int rb_wasm_try(void (* may_throw)(uintptr_t, uintptr_t), uintptr_t arg0, uintptr_t arg1);

int rb_wasm_try_retry_catch(int (* may_throw)(uintptr_t, uintptr_t, int, va_list), uintptr_t arg0, uintptr_t arg1, int state, va_list args);

void throw_left(uintptr_t arg0, uintptr_t arg1) {
  rb_wasm_throw(arg0);
  assert(0 && "unreachable");
}

void throw_sum_value(uintptr_t arg0, uintptr_t arg1) {
  rb_wasm_throw(arg0 + arg1);
  assert(0 && "unreachable");
}

void no_throw(uintptr_t arg0, uintptr_t arg1) {
}

struct context {
  int value;
};

int throw_until_value_to_be_5(uintptr_t arg0, uintptr_t _1, int _2, va_list _3) {
  struct context *ctx = (struct context *)arg0;
  ctx->value++;
  if (ctx->value == 5) {
    return 0;
  } else {
    rb_wasm_throw(1);
  }
}

int inc_state(uintptr_t _0, uintptr_t _1, int state, va_list _3) {
  return state + 1;
}

int main(void) {
  struct context ctx = { .value = 0 };
  int result;
  result = rb_wasm_try(throw_left, 2, 3);
  assert(result == 2);

  result = rb_wasm_try(throw_sum_value, 2, 3);
  assert(result == 5);

  result = rb_wasm_try(no_throw, 2, 3);
  assert(result == 0);

  result = rb_wasm_try_retry_catch(throw_until_value_to_be_5, (uintptr_t)&ctx, 3, 2, NULL);
  assert(result == 0);
  assert(ctx.value == 5);

  result = rb_wasm_try_retry_catch(inc_state, 4, 3, 2, NULL);
  assert(result == 3);

  return 0;
}
