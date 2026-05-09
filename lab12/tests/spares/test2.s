	.file "tests/spares/test2.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $5, %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $1, %rdx
	addq %rdx, %rax
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
