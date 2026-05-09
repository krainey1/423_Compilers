	.file "k0/sem1.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $96, %rsp
	movq $10, %rax
	movq %rax, -8(%rbp)
	movq $3, %rax
	movq %rax, -40(%rbp)
	movq -8(%rbp), %rax
	movq -40(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	movq %rax, -24(%rbp)
	movq -8(%rbp), %rax
	movq -40(%rbp), %rdx
	subq %rdx, %rax
	movq %rax, -72(%rbp)
	movq -72(%rbp), %rax
	movq %rax, -56(%rbp)
	movq -8(%rbp), %rax
	movq -40(%rbp), %rdx
	imulq %rdx, %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rax
	movq %rax, -32(%rbp)
	movq -40(%rbp), %rdx
	movq %rdx, %rcx
	movq -8(%rbp), %rax
	cqto
	idivq %rcx
	movq %rax, -88(%rbp)
	movq -88(%rbp), %rax
	movq %rax, -16(%rbp)
	movq -40(%rbp), %rdx
	movq %rdx, %rcx
	movq -8(%rbp), %rax
	cqto
	idivq %rcx
	movq %rdx, %rax
	movq %rax, -96(%rbp)
	movq -96(%rbp), %rax
	movq %rax, -48(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
