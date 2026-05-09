	.file "k0/syn9.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl sign
	.type sign, @function
sign:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq %rdi, -8(%rbp)
	movq -8(%rbp), %rax
	movq $0, %rdx
	cmpq %rdx, %rax
	setl %al
	movzbq %al, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	testq %rax, %rax
	jne .L10
	jmp .L11
.L10:
	movq $1, %rax
	negq %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	jmp .L12
.L11:
.L12:
	movq $1, %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size sign, .-sign
	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq $3, %rax
	negq %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rdi
	xorl %eax, %eax
	call sign@PLT
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
