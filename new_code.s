.LC0:
	.string	"%d\n"
	.text
	.globl  main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $0 , %rbx
	movq %rbx, -8(%rbp)
.L_main1:
	movq -8(%rbp), %rbx
	movq $5 , %rcx
	cmpq %rbx, %rcx
	jg .L_main0
	jmp .end_main
.L_main0:
	movq -8(%rbp), %rbx
	pushq %rbx
	movl %ebx, %esi
	leaq .LC0(%rip), %rdi
	movq $0, %rax
	call printf@PLT
	movq -8(%rbp), %rbx
	movq $1 , %rcx
	addq %rcx, %rbx
	movq %rbx, -16(%rbp)
	movq -16(%rbp), %rbx
	movq %rbx, -8(%rbp)
	jmp .L_main1
.end_main:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
