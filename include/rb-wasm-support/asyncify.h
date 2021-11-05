#ifndef RB_WASM_SUPPORT_ASYNCIFY_H
#define RB_WASM_SUPPORT_ASYNCIFY_H

__attribute__((import_module("asyncify"), import_name("start_unwind")))
void asyncify_start_unwind(void *buf);
__attribute__((import_module("asyncify"), import_name("stop_unwind")))
void asyncify_stop_unwind(void);
__attribute__((import_module("asyncify"), import_name("start_rewind")))
void asyncify_start_rewind(void *buf);
__attribute__((import_module("asyncify"), import_name("stop_rewind")))
void asyncify_stop_rewind(void);

void rb_asyncify_set_active_buf(void *buf);
void *rb_asyncify_get_active_buf(void);

#endif
