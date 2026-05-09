	.file "k0/syn8.s"
	# k0 compiler -- x86-64 AT&T Linux

	.section .rodata
__str0:
	.string "big"
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
	movq $10, %rax
	movq %rax, -8(%rbp)
	movq -8(%rbp), %rax
	movq $0, %rdx
	cmpq %rdx, %rax
	setg %al
	movzbq %al, %rax
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rax
	testq %rax, %rax
	jne .L9
	jmp .L10
.L9:
	movq -8(%rbp), %rax
	movq $5, %rdx
	cmpq %rdx, %rax
	setg %al
	movzbq %al, %rax
	movq %rax, -24(%rbp)
	movq -24(%rbp), %rax
	testq %rax, %rax
	jne .L12
	jmp .L13
.L12:
	leaq __str0(%rip), %rax
	movq %rax, -32(%rbp)
	movq -32(%rbp), %rdi
	call puts@PLT
	movq %rax, -40(%rbp)
	jmp .L14
.L13:
.L14:
	jmp .L11
.L10:
.L11:
	# epilogue
	movq %rbp, %rsp
	popq %rbp
	ret
	.size main, .-main
	.section .note.GNU-stack,"",@progbits
