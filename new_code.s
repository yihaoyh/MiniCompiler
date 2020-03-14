	.text
	.globl  f
f:
	pushq %rbp
	movq %rsp, %rbp
	movq 16(%rbp), %rbx
	movq $10 , %rcx
	addq %rcx, %rbx
	pushq %rbx
	movq -8(%rbp), %rbx
	pushq %rbx
	movq -16(%rbp), %rax
	addq $16, %rsp
	popq %rbp
	ret
.LC0:
	.string	"%d\n"
	.text
	.globl  main
main:
	pushq %rbp
	movq %rsp, %rbp
	movq $0 , %rbx
	pushq %rbx
	movq -8(%rbp), %rbx
	movq $1 , %rcx
	addq %rcx, %rbx
	pushq %rbx
	movq -16(%rbp), %rbx
	movq $2 , %rcx
	addq %rcx, %rbx
	pushq %rbx
	movq -24(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movq -8(%rbp), %rbx
	pushq %rbx
	call f
	movq %rax, %rbx
	pushq %rbx
	movq -40(%rbp), %rbx
	movq %rbx, -8(%rbp)
	movl %ebx, %esi
	leaq .LC0(%rip), %rdi
	call printf@PLT
	addq $40, %rsp
	popq %rbp
	ret
