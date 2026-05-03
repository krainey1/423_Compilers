	.file	"arith.c"
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
	.globl	z
	.align 4
z:
	.space 4
	.text
	.globl	test
	.def	test;	.scl	2;	.type	32;	.endef
	.seh_proc	test
test:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movl	x(%rip), %edx
	movl	y(%rip), %eax
	addl	%edx, %eax
	movl	%eax, z(%rip)
	movl	x(%rip), %edx
	movl	y(%rip), %eax
	subl	%eax, %edx
	movl	%edx, z(%rip)
	movl	x(%rip), %edx
	movl	y(%rip), %eax
	imull	%edx, %eax
	movl	%eax, z(%rip)
	nop
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
