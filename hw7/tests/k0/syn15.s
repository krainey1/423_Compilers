	.file "k0/syn15.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__fmt_int_ln:
	.string "%ld\n"
__fmt_int:
	.string "%ld"
__empty_str:
	.string ""

	.text

	.globl sum
	.type sum, @function
sum:
	pushq %rbp
	movq %rsp, %rbp
	subq $64, %rsp
	movq %rdi, -8(%rbp)
	movq %rsi, -16(%rbp)
	movq %rdx, -24(%rbp)
	movq $0, %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq -8(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq -16(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -48(%rbp)
	movq -48(%rbp), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq -24(%rbp), %rdx
	addq %rdx, %rax
	movq %rax, -56(%rbp)
	movq -56(%rbp), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size sum, .-sum
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
	call sum@PLT
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	movq %rax, -8(%rbp)
	leaq __fmt_int_ln(%rip), %rdi
	movq -8(%rbp), %rsi
	xorl %eax, %eax
	call printf@PLT
	movq %rax, -24(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
