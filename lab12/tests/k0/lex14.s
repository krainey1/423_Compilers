	.file "k0/lex14.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "default"

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq $0, %rax
	movq %rax, -8(%rbp)
	leaq __str0(%rip), %rax
	movq %rax, -24(%rbp)
	movq -8(%rbp), %rax
	testq %rax, %rax
	jne .L4
	movq -24(%rbp), %rax
	movq %rax, -32(%rbp)
	jmp .L5
.L4:
	movq -8(%rbp), %rax
	movq %rax, -32(%rbp)
.L5:
	movq -32(%rbp), %rax
	movq %rax, -16(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
