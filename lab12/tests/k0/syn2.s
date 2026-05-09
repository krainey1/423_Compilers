	.file "k0/syn2.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl add
	.type add, @function
add:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq %rdi, -8(%rbp)
	movq %rsi, -16(%rbp)
	movq -8(%rbp), %rax
	movq -16(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size add, .-add
	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $2, %rdi
	movq $1, %rsi
	xorl %eax, %eax
	call add@PLT
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
