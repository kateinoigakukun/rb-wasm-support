#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void *rb_wasm_get_stack_pointer(void);

static void *base_stack_pointer = NULL;

int __attribute__((constructor)) record_base_sp(void) {
  base_stack_pointer = rb_wasm_get_stack_pointer();
  return 0;
}

void dump_memory(uint8_t *base, uint8_t *end) {
  size_t chunk_size = 16;

  for (uint8_t *ptr = base; ptr <= end; ptr += chunk_size) {
    printf("%p", ptr);
    for (size_t offset = 0; offset < chunk_size; offset++) {
      printf(" %02x", *(ptr + offset));
    }
    printf("\n");
  }
}

bool find_in_stack(uint32_t target, void *base, void *end) {
  printf("checking %#x ... ", target);

  for (void *ptr = base; ptr <= end; ptr += sizeof(uint32_t)) {
    if ((*(uint32_t *)ptr) == target) {
      printf("ok\n");
      return true;
    }
  }
  return false;
}

void check_stack(void *base, void *end) {
  for (uint32_t i = 1; i <= 5; i++) {
    assert(find_in_stack(i, base, end) && "missing argument value");
  }

  assert(find_in_stack(0xdeadbeef, base, end) && "missing local variable");
  assert(find_in_stack(0xfacefeed, base, end) && "missing local reg variable");
}

void new_frame(uint32_t arg) {
  if (arg == 0) {
    dump_memory(rb_wasm_get_stack_pointer(), base_stack_pointer);
    check_stack(rb_wasm_get_stack_pointer(), base_stack_pointer);
  } else {
    new_frame(arg - 1);
  }
}

int main(void) {
  uint32_t deadbeef;
  register uint32_t facefeed;
  deadbeef = 0xdeadbeef;
  facefeed = 0xfacefeed;

  new_frame(5);

  return 0;
}
