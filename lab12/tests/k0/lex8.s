	.file "k0/lex8.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq $1, %rax
	testq %rax, %rax
	jne .L12
	jmp .L10
.L12:
	movq $0, %rax
	testq %rax, %rax
	jne .L9
	jmp .L10
.L9:
	movq $1, %rax
	movq %rax, -32(%rbp)
	jmp .L11
.L10:
	movq $0, %rax
	movq %rax, -32(%rbp)
.L11:
	movq -32(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $1, %rax
	testq %rax, %rax
	jne .L13
	jmp .L16
.L16:
	movq $0, %rax
	testq %rax, %rax
	jne .L13
	jmp .L14
.L13:
	movq $1, %rax
	movq %rax, -40(%rbp)
	jmp .L15
.L14:
	movq $0, %rax
	movq %rax, -40(%rbp)
.L15:
	movq -40(%rbp), %rax
	movq %rax, -24(%rbp)
	movq $1, %rax
	testq %rax, %rax
	sete %al
	movzbq %al, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
