	.file "k0/syn5.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
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
	subq $32, %rsp
	movq $1, %rax
	movq %rax, -8(%rbp)
.L8:
	movq -8(%rbp), %rax
	movq $5, %rdx
	cmpq %rdx, %rax
	setle %al
	movzbq %al, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	testq %rax, %rax
	jne .L9
	jmp .L10
.L9:
	leaq __fmt_int_ln(%rip), %rdi
	movq -8(%rbp), %rsi
	xorl %eax, %eax
	call printf@PLT
	movq %rax, -24(%rbp)
	addq $1, -8(%rbp)
	jmp .L8
.L10:
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
