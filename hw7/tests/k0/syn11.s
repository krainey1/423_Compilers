	.file "k0/syn11.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl foo
	.type foo, @function
foo:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $1, %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size foo, .-foo
	.globl bar
	.type bar, @function
bar:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $2, %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size bar, .-bar
	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	xorl %eax, %eax
	call foo@PLT
	movq %rax, -16(%rbp)
	xorl %eax, %eax
	call bar@PLT
	movq %rax, -24(%rbp)
	movq -16(%rbp), %rax
	movq -24(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
