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
.end
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
	cmpq %rbx, %rcx
	jl .L0
	jmp .end
.L0:
	movq $2 , %rbx
	movq %rbx, -8(%rbp)
.end
	addq $8, %rsp
	popq %rbp
	ret
