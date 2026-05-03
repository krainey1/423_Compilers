	.file	"test.c"
	.text
	.globl	x
	.bss
	.align 4
x:
	.space 4
	.globl	y
	.align 4
y:
	.space 4
	.section .rdata,"dr"
.LC0:
	.ascii "%d\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	call	__main
	movl	$5, x(%rip)
	movl	x(%rip), %eax
	addl	$3, %eax
	movl	%eax, y(%rip)
	movl	y(%rip), %eax
	leaq	.LC0(%rip), %rcx
	movl	%eax, %edx
	call	printf
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
