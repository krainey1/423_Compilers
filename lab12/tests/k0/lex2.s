	.file "k0/lex2.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "hello world"

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	leaq __str0(%rip), %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -8(%rbp)
	# load_to_rax: unhandled region 0
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
