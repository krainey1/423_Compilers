	.file "k0/lex13.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq $3, %rdi
	movq $2, %rsi
	movq $1, %rdx
	xorl %eax, %eax
	call arrayOf@PLT
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	movq (%rax), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rax, -8(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	# --- runtime stubs for Kotlin builtins ---
	.text
	.weak arrayOf
	.type arrayOf, @function
arrayOf:
	pushq %rbp
	movq %rsp, %rbp
	xorl %eax, %eax
	popq %rbp
	ret
	.size arrayOf, .-arrayOf
	.section .note.GNU-stack,"",@progbits
