#ifndef RB_WASM_SUPPORT_MACHINE_H
#define RB_WASM_SUPPORT_MACHINE_H

typedef void (*rb_wasm_scan_func)(void*, void*);

void rb_wasm_scan_locals(rb_wasm_scan_func scan);

#define rb_wasm_scan_stack(scan) _rb_wasm_scan_stack((scan), rb_wasm_get_stack_pointer())

void _rb_wasm_scan_stack(rb_wasm_scan_func scan, void *current);
/// defined in machine.S
void *rb_wasm_get_stack_pointer(void);

#endif
