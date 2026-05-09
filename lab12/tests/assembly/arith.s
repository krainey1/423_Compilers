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
	#all stuff above is kinda the same general stuff for test.s and hello.s which has already been shown
	# z = x _ y (OP_ADD INT and global var)
	# TAC: t1 = x+y; z = t1
	movl	x(%rip), %edx
	movl	y(%rip), %eax
	addl	%edx, %eax	#%eax = x+y
				#Linux would do leal (%rdx,%rax), %eax but MSYS2 GCC 15 does addl directly
	movl	%eax, z(%rip)	#storing result
	
	# z = x - y (OPT)SUB_INT and global var
	movl	x(%rip), %edx
	movl	y(%rip), %eax
	subl	%eax, %edx	#subtracts eax from edx such that %edx = x-y
	movl	%edx, z(%rip)	#storing result
	
	# z= x*y
	movl	x(%rip), %edx
	movl	y(%rip), %eax
	imull	%edx, %eax	# signed multiply and store into %eax
				# divis would need idivl and sign-extension with cltd/cdq
	movl	%eax, z(%rip)
	nop

	#func epilogic (TAC: endproc test)
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
# ---
# summary thing: TAC opcode -> x86-64 mapping observed (global int variables)
#
#  OP_GLOBAL x     ->  .globl x / .bss / .align 4 / x: / .space 4
#  OP_ASSIGN (const)->  movl $val, x(%rip)
#  OP_ADD_INT      ->  movl a(%rip),%edx / movl b(%rip),%eax / addl %edx,%eax / movl %eax,dst(%rip)
#  OP_SUB_INT      ->  movl a(%rip),%edx / movl b(%rip),%eax / subl %eax,%edx / movl %edx,dst(%rip)
#  OP_MUL_INT      ->  movl a(%rip),%edx / movl b(%rip),%eax / imull %edx,%eax / movl %eax,dst(%rip)
#  OP_PROC         ->  pushq %rbp / movq %rsp,%rbp
#  OP_ENDPROC      ->  popq %rbp / ret