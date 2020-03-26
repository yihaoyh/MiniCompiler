	.text
	.globl  f
f:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
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
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	pushq %rbx
	call f
	addq $8 ,%rsp
	movq %rax, -16(%rbp)
	movq 16(%rbp), %rbx
	movq -16(%rbp), %rcx
	addq %rcx, %rbx
	movq %rbx, -24(%rbp)
	movq -24(%rbp), %rax
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
	subq $16, %rsp
	movq $2 , %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	pushq %rbx
	call f
	addq $8 ,%rsp
	movq %rax, -16(%rbp)
	movq -16(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	pushq %rbx
	movl %ebx, %esi
	leaq .LC0(%rip), %rdi
	movq $0, %rax
	call printf@PLT
.end_main:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
