	.file "k0/sem10.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "fallback"
__fmt_int_ln:
	.string "%ld\n"
__fmt_int:
	.string "%ld"
__empty_str:
	.string ""

	.text

	.globl main
	.type main, @function
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq $0, %rax
	movq %rax, -8(%rbp)
	leaq __str0(%rip), %rax
	movq %rax, -24(%rbp)
	movq -8(%rbp), %rax
	testq %rax, %rax
	jne .L6
	movq -24(%rbp), %rax
	movq %rax, -32(%rbp)
	jmp .L7
.L6:
	movq -8(%rbp), %rax
	movq %rax, -32(%rbp)
.L7:
	movq -32(%rbp), %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rdi
	call puts@PLT
	movq %rax, -40(%rbp)
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
