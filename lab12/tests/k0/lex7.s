	.file "k0/lex7.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $96, %rsp
	movq $1, %rax
	movq $2, %rdx
	cmpq %rdx, %rax
	setl %al
	movzbq %al, %rax
	movq %rax, -56(%rbp)
	movq -56(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $2, %rax
	movq $1, %rdx
	cmpq %rdx, %rax
	setg %al
	movzbq %al, %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	movq %rax, -40(%rbp)
	movq $1, %rax
	movq $1, %rdx
	cmpq %rdx, %rax
	setle %al
	movzbq %al, %rax
	movq %rax, -72(%rbp)
	movq -72(%rbp), %rax
	movq %rax, -24(%rbp)
	movq $2, %rax
	movq $2, %rdx
	cmpq %rdx, %rax
	setge %al
	movzbq %al, %rax
	movq %rax, -80(%rbp)
	movq -80(%rbp), %rax
	movq %rax, -48(%rbp)
	movq $1, %rax
	movq $1, %rdx
	cmpq %rdx, %rax
	sete %al
	movzbq %al, %rax
	movq %rax, -88(%rbp)
	movq -88(%rbp), %rax
	movq %rax, -32(%rbp)
	movq $1, %rax
	movq $2, %rdx
	cmpq %rdx, %rax
	setne %al
	movzbq %al, %rax
	movq %rax, -96(%rbp)
	movq -96(%rbp), %rax
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
