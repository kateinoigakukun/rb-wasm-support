#include <stdio.h>
#include <assert.h>
#include <stdint.h>

void *rb_wasm_get_stack_pointer(void);

static void *base_stack_pointer = NULL;

int __attribute__((constructor)) record_base_sp(void) {
  base_stack_pointer = rb_wasm_get_stack_pointer();
  return 0;
}

void dump_memory(uint8_t *base, uint8_t *end) {
  int chunk_size = 16;

  for (uint8_t *ptr = base; ptr <= end; ptr += chunk_size) {
    printf("%p", ptr);
    for (int offset = 0; offset < chunk_size; offset++) {
      printf(" %02x", *(ptr + offset));
    }
    printf("\n");
  }
}

#define DUMP_STACK_SPACE() do { \
  dump_memory(rb_wasm_get_stack_pointer(), base_stack_pointer); \
} while (0)

void new_frame(int arg) {
  if (arg == 0) {
    DUMP_STACK_SPACE();
  } else {
    new_frame(arg - 1);
  }
}

int main(void) {
  int deadbeef;
  register int facefeed;
  deadbeef = 0xdeadbeef;
  facefeed = 0xfacefeed;

  new_frame(5);

  return 0;
}
