	.file "k0/sem7.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "hello"

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $0, %rax
	movq %rax, -8(%rbp)
	leaq __str0(%rip), %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
