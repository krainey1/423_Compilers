	.file	"control.c"
	.text
	.globl	foo
	.def	foo;	.scl	2;	.type	32;	.endef
	.seh_proc	foo
foo:				#TAC: proc foo which is int foo(intx)
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue

	#Receive param x
	movl	%ecx, 16(%rbp)
	#OP_JUMP_FALSE/OP_JUMP_TRUE if (x>0)
	cmpl	$0, 16(%rbp)	#compare x with 0
				# also sets condition flags: ZF, SF, OF, etc.
				# TAC: comparison is x>0 so check if x<= to SKIP
	jle	.L2		# jump if less than or equal,
				# TAC: OP_JUMP_FALSE if not x>0 to jle.L2
	# then branch ret 1
	movl	$1, %eax	#TAC: OP_ASSIGN result, const(1)
	jmp	.L3		# TAC: OP_JUMP .L3 (skip L3 by jumping over block)
.L2:
	movl	$0, %eax	#else branch falls through to .L3
.L3:				#TAC: OP_LABLE .L3 shared func exit
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
# another summary thing: TAC opcode -> x86-64 mapping observed (control flow)
#
#  if (x > 0) compiles as:
#    cmpl  $0, x_location     # compare x to 0
#    jle   .L_else             # jump if condition is FALSE (inverted!)
#    <then block>
#    jmp   .L_end              # OP_JUMP: skip else
#  .L_else:                    # OP_LABEL
#    <else block>
#  .L_end:                     # OP_LABEL (follow label)
#
#  Impt to note: gcc INVERTS the branch condition and jumps to the ELSE.
#  So "if x > 0" becomes "jle" (jump if NOT > 0).
#  Full condition code jump table:
#    >   (gt)  -> jle to skip   |   <=  (lte) -> jg  to skip
#    <   (lt)  -> jge to skip   |   >=  (gte) -> jl  to skip
#    ==  (eq)  -> jne to skip   |   !=  (neq) -> je  to skip
#
#  OP_JUMP        ->  jmp .Ln
#  OP_JUMP_TRUE   ->  cmpq $0, val / jne .Ln   (jump if nonzero = true)
#  OP_JUMP_FALSE  ->  cmpq $0, val / je  .Ln   (jump if zero = false)
#  OP_LABEL Ln    ->  .Ln:
#  OP_RETURN x    ->  movl $x, %eax  (then fall through to epilogue)