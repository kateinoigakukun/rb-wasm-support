	.text
	.file	"setjmp.S"

	.functype	_rb_wasm_setjmp_internal (i32) -> (i32)
	.globaltype 	__stack_pointer, i32

# ========= START OF _rb_wasm_setjmp FUNCTION ==============
# A wrapper of _rb_wasm_setjmp_internal to save and restore stack pointer
# This cannot be implemented in C because there is no way to manipulate stack pointer
# without C-epilogue.
	.section	.text._rb_wasm_setjmp,"",@
	.globl		_rb_wasm_setjmp
	.type		_rb_wasm_setjmp,@function
_rb_wasm_setjmp:
	.functype	_rb_wasm_setjmp (i32) -> (i32)
	.local		i32, i32
	# save sp (this local is stored in asyncify stack and restored when rewinding)
	global.get	__stack_pointer
	local.set	1

	local.get	0
	call		_rb_wasm_setjmp_internal

	# restore sp
	local.get	1
	global.set	__stack_pointer
	end_function
.L_rb_wasm_setjmp_end:
	.size		_rb_wasm_setjmp, .L_rb_wasm_setjmp_end-_rb_wasm_setjmp
# ========= END OF _rb_wasm_setjmp FUNCTION ==============
