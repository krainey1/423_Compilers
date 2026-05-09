	.file	"hello.c"
	.text
	.section .rdata,"dr"	#read only data section in Windows/MSYS2
.LC0:
	.ascii "Hello World!\0"	#gcc on windows uses .ascii for null term string
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef	#Windows COFF debug info which replaces Linux .type main,@function
	.seh_proc	main					# Windows struc exception handling prologue marker whiich replaces Linux .cfi_startproc
main:				#function entry label (TAC: proc main)
	pushq	%rbp		# save old base pointer on stack
	.seh_pushreg	%rbp	# the SEH annocation for the push above
	movq	%rsp, %rbp	# set frame pointer as curent stack pointer
	.seh_setframe	%rbp, 0	# SEH record frame reg and offset
	subq	$32, %rsp	# that 32 byte shadow space requirement from Windows like in test.s; happens before nay call and also doesn't exist in Linux
	.seh_stackalloc	32	# SEH annotation for stack alloc
	.seh_endprologue
	call	__main		# Windows intiializes C runtime (mingw startup or in my case UCRT64)
	leaq	.LC0(%rip), %rax	#load hello world string addr into %rax also %rip-relative addr is addr = current PC + offset
	movq	%rax, %rcx	# move str pointer into %rcx
				# Windows and Linux here so it doesn't quite match the linux table
				# Linux: arg1 -> %rdi, arg2 -> %rsi, arg3 -> %rdx
				# Windows: arg1 -> %rcx, arg2 -> %rdx, arg3 -> %r8
				# TACL param "Hello World!" -> movq addr, %rcx (Windows)
	# rest is all kind of the same with printf and setting return value to 0 along with windows shadow space dealloc
	call	printf
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
