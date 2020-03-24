	.text
	.globl  f
f:
	pushq %rbp
	movq %rsp, %rbp
	movq 16(%rbp), %rbx
	movq $0 , %rcx
	cmpq %rbx, %rcx
	je .L_f0
	jmp .L_f1
.L_f0:
	movq 16(%rbp), %rax
	jmp .end_f
.L_f1:
	movq 16(%rbp), %rbx
	movq $1 , %rcx
	subq %rcx, %rbx
	pushq %rbx
	movq -8(%rbp), %rbx
	pushq %rbx
	call f
	movq %rax, %rbx
	pushq %rbx
	movq 16(%rbp), %rbx
	movq -24(%rbp), %rcx
	addq %rcx, %rbx
	pushq %rbx
	movq -32(%rbp), %rax
	jmp .end_f
.end_f:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
.LC0:
	.string	"%d\n"
	.text
	.globl  main
main:
	pushq %rbp
	movq %rsp, %rbp
	movq $2 , %rbx
	pushq %rbx
	movq -8(%rbp), %rbx
	pushq %rbx
	call f
	movq %rax, %rbx
	pushq %rbx
	movq -24(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	pushq %rbx
	movl %ebx, %esi
	leaq	.LC0(%rip), %rdi
	call	printf@PLT
.end_main:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
