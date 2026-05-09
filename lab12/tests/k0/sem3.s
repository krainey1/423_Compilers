	.file "k0/sem3.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $64, %rsp
	movq $1, %rax
	movq %rax, -8(%rbp)
	movq $0, %rax
	movq %rax, -32(%rbp)
	movq -8(%rbp), %rax
	testq %rax, %rax
	jne .L16
	jmp .L14
.L16:
	movq -32(%rbp), %rax
	testq %rax, %rax
	jne .L13
	jmp .L14
.L13:
	movq $1, %rax
	movq %rax, -48(%rbp)
	jmp .L15
.L14:
	movq $0, %rax
	movq %rax, -48(%rbp)
.L15:
	movq -48(%rbp), %rax
	movq %rax, -16(%rbp)
	movq -8(%rbp), %rax
	testq %rax, %rax
	jne .L17
	jmp .L20
.L20:
	movq -32(%rbp), %rax
	testq %rax, %rax
	jne .L17
	jmp .L18
.L17:
	movq $1, %rax
	movq %rax, -56(%rbp)
	jmp .L19
.L18:
	movq $0, %rax
	movq %rax, -56(%rbp)
.L19:
	movq -56(%rbp), %rax
	movq %rax, -40(%rbp)
	movq -8(%rbp), %rax
	testq %rax, %rax
	sete %al
	movzbq %al, %rax
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rax
	movq %rax, -24(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
