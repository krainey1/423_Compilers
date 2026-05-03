	.file	"test.c"
	.text

	# These are Global Var Declarations
	# TAC: OP_ GLOBAL x, OP_GLOBAL y
	.globl	x
	.bss		#uninit data section
	.align 4
x:
	.space 4	# 4 bytes for int x (TAC with 8 for 64-bit, C int is 4)
	.globl	y
	.align 4
y:
	.space 4	# 4 bytes for int y

	# String const for printf format String
	# TAC: OP_STRING  as .section .rdata + label + .ascii
	.section .rdata,"dr"		# read-only data (using Windows so it's .rdata as opposed to Linux .rodata)
.LC0:
	.ascii "%d\12\0"		#format string where \12 is cotal for newline and \0 us null term
					# TAC: addr_string(0) would ref this label
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:					# TAC: have this as proc main
	pushq	%rbp			# save old base pointer
	.seh_pushreg	%rbp
	movq	%rsp, %rbp		# establish stack frame
	.seh_setframe	%rbp, 0
	subq	$32, %rsp		# allocate 32 byts shadow space which is a Windows req
	.seh_stackalloc	32
	.seh_endprologue
	call	__main			# Windows C runtime init (not in TAC)
	
	# OP_ASSIGN, global, immediate const (x = 5)
	movl	$5, x(%rip)		#store const 5 directly into global x
					# TAC: OP_ASSIGN x, const(5)
					# $5 = immediate val 5
					# x(%rip) = RIP-relative addr of global x
	# OPT_ADD_INT with one immediate operand (y = x+3)
	movl	x(%rip), %eax		# load global x into %eax
					# TAC: OP_ASSIGN t1, x	
	addl	$3, %eax		# %eax = x + 3
					# TAC: OP_ADD_INT t2, t1, const(3)
					# gcc will omptimize x+3 into addl with immediate no need to load 3 into reg first		
	movl	%eax, y(%rip)		# store res into global y
					# TAC: OP_ASSIGN y, t2
	# OP PARM x2 + OP_CALL ( the print statement)
	movl	y(%rip), %eax		# load y into %eax as scratch
	leaq	.LC0(%rip), %rcx	# arg1 = addr format string %rcx
					# TAC: OP PARM "%d\n" -> leaq .LC0(%rip), %rcx
					# leaq is just load effective addr (pointer not val)
	movl	%eax, %edx		# arg2 = y into %edx
					# TAC: OP_PARM y into mov1 y(%rip), %edx
					# Windows arg order: arg1=%rcx, arg2=%rdx		
	call	printf			# TAC: OP_CALL printf, 2
	nop				# padding stuff
	addq	$32, %rsp		$ dealloc the shadow space
	popq	%rbp			# restore base pointer
	ret				# TAC: OP_ENDPROC main
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
# --------------------------------------------------------
# Mapping summary from full TAC to x86-64 (globals + string + printf call)
#
#  OP_GLOBAL x         ->  .globl x / .bss / .align 4 / x: / .space 4
#  OP_STRING "..."     ->  .section .rdata / .LCn: / .ascii "...\0"
#  OP_ASSIGN x,const   ->  movl $const, x(%rip)
#  OP_ASSIGN x,y(glob) ->  movl y(%rip),%eax / movl %eax,x(%rip)
#  OP_ADD_INT (immed)  ->  movl x(%rip),%eax / addl $3,%eax / movl %eax,y(%rip)
#  OP_PARM str         ->  leaq .LCn(%rip), %rcx   (1st arg, Windows)
#  OP_PARM intval      ->  movl val, %edx           (2nd arg, Windows)
#  OP_CALL printf,2    ->  call printf
#  OP_PROC main        ->  pushq %rbp / movq %rsp,%rbp / subq $32,%rsp
#  OP_ENDPROC main     ->  addq $32,%rsp / popq %rbp / ret