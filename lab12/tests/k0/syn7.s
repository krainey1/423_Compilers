	.file "k0/syn7.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__fmt_int_ln:
	.string "%ld\n"
__fmt_int:
	.string "%ld"
__empty_str:
	.string ""

	.text

	.globl double
	.type double, @function
double:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq %rdi, -8(%rbp)
	movq -8(%rbp), %rax
	movq $2, %rdx
	imulq %rdx, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size double, .-double
	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $21, %rdi
	xorl %eax, %eax
	call double@PLT
	movq %rax, -8(%rbp)
	leaq __fmt_int_ln(%rip), %rdi
	movq -8(%rbp), %rsi
	xorl %eax, %eax
	call printf@PLT
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
