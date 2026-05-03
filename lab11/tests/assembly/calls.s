	.file	"calls.c"
	.text
	.globl	add
	.def	add;	.scl	2;	.type	32;	.endef
	.seh_proc	add
add:					#TAC: proc add as entry point for functino add
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	
	# other stuff above kinda also the same as before, but below is where Windows calling is different from linux
	# Windows x64: arg1=%rcx/%ecx, arg2=%rdx/%edx, arg3=%r9d, arg4=%r9d
	# Linux x86-64: arg1=%rdi/%edi, arg2=%rsi/%esi, arg3=%edx, arg4%ecx
	# and then it's more of that shadow space stuff for Windows
	movl	%ecx, 16(%rbp)
	movl	%edx, 24(%rbp)
	
	#return a+b OP_ADD_INT  and OP_RETURN
	movl	16(%rbp), %edx	#reload a from shadow space
	movl	24(%rbp), %eax	#reload b also from shadow space
	addl	%edx, %eax	# matches TAC: OP_RETURN x where result in %eax and return
	popq	%rbp
	ret			# return and %eax holds that val
	.seh_endproc
	.globl	test
	.def	test;	.scl	2;	.type	32;	.endef
	.seh_proc	test
test:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	
	# r = add(3,4)
	movl	$4, %edx
	movl	$3, %ecx
	# TAC does OP_PARM argument assignment in reverse order but the registers matter not load order
	call	add
	movl	%eax, -4(%rbp)	#store ret val into local var r
				# TAC: OP_ASSIGN r, res of call
				# r is at -4(%rbp) which is first local, 4 bytes from frame top
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
# sum stuff: TAC opcode -> x86-64 mapping observed (function calls)
#
#  OP_PROC fname   ->  pushq %rbp / movq %rsp,%rbp / subq $N,%rsp
#  OP_PARM arg1    ->  movl val, %ecx   (Windows) / %edi (Linux)
#  OP_PARM arg2    ->  movl val, %edx   (Windows) / %esi (Linux)
#  OP_CALL fname,n ->  call fname  (result lands in %eax)
#  OP_RETURN x     ->  result already in %eax from addl; then ret
#  OP_ASSIGN local ->  movl %eax, -N(%rbp)
#  OP_ENDPROC      ->  addq $N,%rsp / popq %rbp / ret
#
#  Windows register order for parameters (differs from Linux but shouldn't matter rlly):
#    Param#:  1      2      3     4
#    32-bit: %ecx  %edx   %r8d  %r9d
#    64-bit: %rcx  %rdx   %r8   %r9
#
#  Linux register order (from assignment table):
#    Param#:  1      2      3     4     5     6
#             %rdi  %rsi   %rdx  %rcx  %r8   %r9
