	.file "k0/syn10.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq $0, %rax
	movq %rax, -8(%rbp)
.L11:
	movq -8(%rbp), %rax
	movq $10, %rdx
	cmpq %rdx, %rax
	setl %al
	movzbq %al, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	testq %rax, %rax
	jne .L12
	jmp .L13
.L12:
	movq -8(%rbp), %rax
	movq $5, %rdx
	cmpq %rdx, %rax
	sete %al
	movzbq %al, %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	testq %rax, %rax
	jne .L14
	jmp .L15
.L14:
	jmp .L13
	jmp .L16
.L15:
.L16:
	addq $1, -8(%rbp)
	jmp .L11
.L13:
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
