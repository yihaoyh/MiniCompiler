.LC0:
	.string	"%d\n"
	.text
	.globl  main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $160, %rsp
	movq $0 , %rax
	movq $8 , %rbx
	mulq %rbx
	movq %rax, -64(%rbp)
	movq -64(%rbp), %rbx
	leaq -40(%rbp, %rbx), %rbx
	movq %rbx, -72(%rbp)
	movq $0 , %rbx
	movq -72(%rbp), %rcx
	movq %rbx, (%rcx)
	movq $1 , %rax
	movq $8 , %rbx
	mulq %rbx
	movq %rax, -104(%rbp)
	movq -104(%rbp), %rbx
	leaq -40(%rbp, %rbx), %rbx
	movq %rbx, -112(%rbp)
	movq $1 , %rbx
	movq -112(%rbp), %rcx
	movq %rbx, (%rcx)
	movq $2 , %rax
	movq $8 , %rbx
	mulq %rbx
	movq %rax, -120(%rbp)
	movq -120(%rbp), %rbx
	leaq -40(%rbp, %rbx), %rbx
	movq %rbx, -128(%rbp)
	movq $2 , %rbx
	movq -128(%rbp), %rcx
	movq %rbx, (%rcx)
	movq $3 , %rax
	movq $8 , %rbx
	mulq %rbx
	movq %rax, -136(%rbp)
	movq -136(%rbp), %rbx
	leaq -40(%rbp, %rbx), %rbx
	movq %rbx, -144(%rbp)
	movq $3 , %rbx
	movq -144(%rbp), %rcx
	movq %rbx, (%rcx)
	movq $4 , %rax
	movq $8 , %rbx
	mulq %rbx
	movq %rax, -152(%rbp)
	movq -152(%rbp), %rbx
	leaq -40(%rbp, %rbx), %rbx
	movq %rbx, -160(%rbp)
	movq $4 , %rbx
	movq -160(%rbp), %rcx
	movq %rbx, (%rcx)
	movq $0 , %rbx
	movq %rbx, -48(%rbp)
.L_main1:
	movq -48(%rbp), %rbx
	movq $5 , %rcx
	cmpq %rbx, %rcx
	jg .L_main0
	jmp .end_main
.L_main2:
	movq -48(%rbp), %rbx
	movq $1 , %rcx
	addq %rcx, %rbx
	movq %rbx, -80(%rbp)
	movq -80(%rbp), %rbx
	movq %rbx, -48(%rbp)
	jmp .L_main1
.L_main0:
	movq -48(%rbp), %rax
	movq $8 , %rbx
	mulq %rbx
	movq %rax, -88(%rbp)
	movq -88(%rbp), %rbx
	leaq -40(%rbp, %rbx), %rbx
	movq %rbx, -96(%rbp)
	movq -96(%rbp), %rbx
	movq (%rbx), %rbx
	movq %rbx, -56(%rbp)
	movq -56(%rbp), %rbx
	pushq %rbx
	movl %ebx, %esi
	leaq .LC0(%rip), %rdi
	movq $0, %rax
	call printf@PLT
	jmp .L_main2
.end_main:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
