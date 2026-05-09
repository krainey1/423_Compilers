	.file "tests/spares/test3.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "{n} is negative"
__str16:
	.string "{n} is zero"
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
	movq $5, %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $0, %rdx
	cmpq %rdx, %rax
	setl %al
	movzbq %al, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	testq %rax, %rax
	jne .L7
	jmp .L8
.L7:
	leaq __str0(%rip), %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rdi
	call puts@PLT
	movq %rax, -32(%rbp)
	jmp .L9
.L8:
	leaq __str16(%rip), %rax
	movq %rax, -40(%rbp)
	movq -40(%rbp), %rdi
	call puts@PLT
	movq %rax, -48(%rbp)
.L9:
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
