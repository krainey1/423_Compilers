	.file "k0/syn12.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq $5, %rax
	negq %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -8(%rbp)
	movq $0, %rax
	testq %rax, %rax
	sete %al
	movzbq %al, %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
