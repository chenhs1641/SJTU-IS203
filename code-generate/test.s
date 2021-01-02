# start of generated code
	.section		.rodata	
	.text	
	.globl	main
	.type	main, @function
main:
	pushq	 %rbp
	movq	%rsp, %rbp
	pushq	 %rbx
	pushq	 %r10
	pushq	 %r11
	pushq	 %r12
	pushq	 %r13
	pushq	 %r14
	pushq	 %r15
	subq	$8, %rsp
	movq	$4, %rax
	movq	%rax, -64(%rbp)
.POS0:
	subq	$8, %rsp
	movq	$1, %rax
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rax
	testq	%rax, %rax
	jz	 .POS1
	subq	$8, %rsp
	movq	$1, %rax
	movq	%rax, -80(%rbp)
	jmp	 .POS1
	jmp	 .POS0
	subq	$8, %rsp
	movq	$2, %rax
	movq	%rax, -88(%rbp)
	jmp	 .POS0
.POS1:
	subq	$8, %rsp
	movq	$3, %rax
	movq	%rax, -96(%rbp)
	subq	$8, %rsp
	movq	$22331, %rax
	movq	%rax, -104(%rbp)
.POS2:
	subq	$8, %rsp
	movq	$0, %rax
	movq	%rax, -112(%rbp)
	movq	-112(%rbp), %rax
	testq	%rax, %rax
	jz	 .POS4
	subq	$8, %rsp
	movq	$35, %rax
	movq	%rax, -120(%rbp)
	jmp	 .POS3
	subq	$8, %rsp
	movq	$2222222, %rax
	movq	%rax, -128(%rbp)
.POS5:
	subq	$8, %rsp
	movq	$1, %rax
	movq	%rax, -136(%rbp)
	movq	-136(%rbp), %rax
	testq	%rax, %rax
	jz	 .POS7
	subq	$8, %rsp
	movq	$163, %rax
	movq	%rax, -144(%rbp)
	jmp	 .POS6
	jmp	 .POS7
	subq	$8, %rsp
	movq	$361, %rax
	movq	%rax, -152(%rbp)
.POS6:
	jmp	 .POS5
.POS7:
	jmp	 .POS4
	subq	$8, %rsp
	movq	$53, %rax
	movq	%rax, -160(%rbp)
.POS3:
	subq	$8, %rsp
	movq	$121, %rax
	movq	%rax, -168(%rbp)
	jmp	 .POS2
.POS4:
	popq	 %r15
	popq	 %r14
	popq	 %r13
	popq	 %r12
	popq	 %r11
	popq	 %r10
	popq	 %rbx
	leave	
	ret	
	.size	main, .-main

# end of generated code
