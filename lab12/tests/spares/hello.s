	.file "tests/spares/hello.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "Hello, World!"
__fmt_int_ln:
	.string "%ld\n"
__fmt_int:
	.string "%ld"
__empty_str:
	.string ""

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	leaq __str0(%rip), %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rdi
	call puts@PLT
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
