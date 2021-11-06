	.text
	.file	"scan_stack.S"

	.globaltype __stack_pointer, i32

# ========= START OF rb_wasm_get_stack_pointer FUNCTION ==============
	.section	.text.rb_wasm_get_stack_pointer,"",@
	.globl	rb_wasm_get_stack_pointer
	.type	rb_wasm_get_stack_pointer,@function
rb_wasm_get_stack_pointer:
	.functype	rb_wasm_get_stack_pointer () -> (i32)
	global.get	__stack_pointer
	end_function
.Lrb_wasm_get_stack_pointer_end:
	.size	rb_wasm_get_stack_pointer, .Lrb_wasm_get_stack_pointer_end-rb_wasm_get_stack_pointer
# ========= END OF rb_wasm_get_stack_pointer FUNCTION ==============

# ========= START OF rb_wasm_set_stack_pointer FUNCTION ==============
	.section	.text.rb_wasm_set_stack_pointer,"",@
	.globl	rb_wasm_set_stack_pointer
	.type	rb_wasm_set_stack_pointer,@function
rb_wasm_set_stack_pointer:
	.functype	rb_wasm_set_stack_pointer (i32) -> ()
	local.get	0
	global.set	__stack_pointer
	end_function
.Lrb_wasm_set_stack_pointer_end:
	.size	rb_wasm_set_stack_pointer, .Lrb_wasm_set_stack_pointer_end-rb_wasm_set_stack_pointer
# ========= END OF rb_wasm_set_stack_pointer FUNCTION ==============

