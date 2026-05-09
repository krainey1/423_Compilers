	.file "k0/sem4.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "hello"
__str6:
	.string " world"

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	leaq __str0(%rip), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -24(%rbp)
	leaq __str6(%rip), %rax
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rax
	movq %rax, -16(%rbp)
	movq -24(%rbp), %rax
	movq -16(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
