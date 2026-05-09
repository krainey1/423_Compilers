	.file "k0/lex9.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq $10, %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $5, %rdx
	addq %rdx, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $2, %rdx
	subq %rdx, %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $3, %rdx
	imulq %rdx, %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $2, %rdx
	movq %rdx, %rcx
	movq -8(%rbp), %rax
	cqto
	idivq %rcx
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $4, %rdx
	movq %rdx, %rcx
	movq -8(%rbp), %rax
	cqto
	idivq %rcx
	movq %rdx, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
