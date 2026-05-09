	.file "k0/lex6.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $80, %rsp
	movq $10, %rax
	movq $2, %rdx
	addq %rdx, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $10, %rax
	movq $2, %rdx
	subq %rdx, %rax
	movq %rax, -56(%rbp)
	movq -56(%rbp), %rax
	movq %rax, -32(%rbp)
	movq $10, %rax
	movq $2, %rdx
	imulq %rdx, %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	movq %rax, -16(%rbp)
	movq $2, %rdx
	movq %rdx, %rcx
	movq $10, %rax
	cqto
	idivq %rcx
	movq %rax, -72(%rbp)
	movq -72(%rbp), %rax
	movq %rax, -40(%rbp)
	movq $3, %rdx
	movq %rdx, %rcx
	movq $10, %rax
	cqto
	idivq %rcx
	movq %rdx, %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rax
	movq %rax, -24(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
