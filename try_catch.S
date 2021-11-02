	.text
	.file	"try_catch.S"

# ========= START OF rb_wasm_throw FUNCTION ==============
	.section	.text.rb_wasm_throw,"",@
	.globl	rb_wasm_throw
	.type	rb_wasm_throw,@function
rb_wasm_throw:
	.functype	rb_wasm_throw (i32) -> ()
	local.get	0
	throw   	__ruby_setjmp
	end_function
.Lrb_wasm_throw_end:
	.size	rb_wasm_throw, .Lrb_wasm_throw_end-rb_wasm_throw
# ========= END OF rb_wasm_throw FUNCTION ==============

# ========= START OF rb_wasm_try FUNCTION ==============
	.section	.text.rb_wasm_try,"",@
	.globl	rb_wasm_try
	.type	rb_wasm_try,@function
rb_wasm_try:
	.functype	rb_wasm_try (i32, i32, i32) -> (i32)
	try
	  local.get 1 # i32 arg0
	  local.get 2 # i32 arg1
	  local.get 0 # callee function ptr
	  call_indirect (i32, i32) -> ()
	catch	__ruby_setjmp
	  return
	end_try
	i32.const 0
	end_function
.Lfunc_end1:
	.size	rb_wasm_try, .Lfunc_end1-rb_wasm_try
# ========= END OF rb_wasm_try FUNCTION ==============

# ========= START OF rb_wasm_try2 FUNCTION ==============
	.section	.text.rb_wasm_try2,"",@
	.globl	rb_wasm_try2
	.type	rb_wasm_try2,@function
rb_wasm_try2:
	.functype	rb_wasm_try2 (i32, i32, i32, i32) -> (i32)
	try
	  local.get 1 # i32 arg0
	  local.get 2 # i32 arg1
	  local.get 3 # i32 arg2
	  local.get 0 # callee function ptr
	  call_indirect (i32, i32, i32) -> ()
	catch	__ruby_setjmp
	  return
	end_try
	i32.const 0
	end_function
.Lrb_wasm_try2_end1:
	.size	rb_wasm_try2, .Lrb_wasm_try2_end1-rb_wasm_try2
# ========= END OF rb_wasm_try2 FUNCTION ==============

# ========= START OF rb_wasm_try_retry_catch FUNCTION ==============
	.section	.text.rb_wasm_try_retry_catch,"",@
	.globl	rb_wasm_try_retry_catch
	.type	rb_wasm_try_retry_catch,@function
rb_wasm_try_retry_catch:
	.functype	rb_wasm_try_retry_catch (i32, i32, i32, i32, i32) -> (i32)
	loop
	  try
	    local.get 1 # i32 arg0
	    local.get 2 # i32 arg1
	    local.get 3 # i32 arg2 (state)
	    local.get 4 # i32 arg3
	    local.get 0 # callee function ptr
	    call_indirect (i32, i32, i32, i32) -> (i32)
	    return
	  catch	__ruby_setjmp
	    local.set 3
	    br 1
	  end_try
	end_loop
	unreachable
	end_function
.Lfunc_end2:
	.size	rb_wasm_try_retry_catch, .Lfunc_end2-rb_wasm_try_retry_catch
# ========= END OF rb_wasm_try_retry_catch FUNCTION ==============

# ========= START OF rb_wasm_try_retry_catch2 FUNCTION ==============
	.section	.text.rb_wasm_try_retry_catch2,"",@
	.globl	rb_wasm_try_retry_catch2
	.type	rb_wasm_try_retry_catch2,@function
rb_wasm_try_retry_catch2:
	.functype	rb_wasm_try_retry_catch2 (i32, i32, i32, i32, i32) -> ()
	loop
	  try
	    local.get 1 # i32 arg0
	    local.get 2 # i32 arg1
	    local.get 3 # i32 arg2 (state)
	    local.get 4 # i32 arg3
	    local.get 0 # callee function ptr
	    call_indirect (i32, i32, i32, i32) -> ()
	    return
	  catch	__ruby_setjmp
	    local.set 3
	    br 1
	  end_try
	end_loop
	unreachable
	end_function
.Lrb_wasm_try_retry_catch2_end2:
	.size	rb_wasm_try_retry_catch2, .Lrb_wasm_try_retry_catch2_end2-rb_wasm_try_retry_catch2
# ========= END OF rb_wasm_try_retry_catch2 FUNCTION ==============


__ruby_setjmp:
	.eventtype	__ruby_setjmp i32

	.section	.custom_section.target_features,"",@
	.int8	1
	.int8	43
	.int8	18
	.ascii	"exception-handling"
