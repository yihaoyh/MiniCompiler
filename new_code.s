	.text
	.globl  fibonacci
fibonacci:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movq 32(%rbp), %rbx
	movq $10 , %rcx
	cmpq %rbx, %rcx
	je .L_fibonacci0
	jmp .L_fibonacci1
.L_fibonacci0:
	movq $0 , %rax
	jmp .end_fibonacci
.L_fibonacci1:
	movq 16(%rbp), %rbx
	movq 24(%rbp), %rcx
	addq %rcx, %rbx
	movq %rbx, -16(%rbp)
	movq -16(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	pushq %rbx
	movl %ebx, %esi
	leaq .LC0(%rip), %rdi
	movq $0, %rax
	call printf@PLT
	movq 32(%rbp), %rbx
	movq $1 , %rcx
	addq %rcx, %rbx
	movq %rbx, -24(%rbp)
	movq -24(%rbp), %rbx
	pushq %rbx
	movq -8(%rbp), %rbx
	pushq %rbx
	movq 24(%rbp), %rbx
	pushq %rbx
	call fibonacci
	addq $24 ,%rsp
	movq %rax, -32(%rbp)
.end_fibonacci:
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
	subq $8, %rsp
	movq $1 , %rbx
	pushq %rbx
	movq $1 , %rbx
	pushq %rbx
	movq $0 , %rbx
	pushq %rbx
	call fibonacci
	addq $24 ,%rsp
	movq %rax, -8(%rbp)
.end_main:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
