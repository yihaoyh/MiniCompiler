.LC0:
	.string	"%d\n"
	.text
	.globl  main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
.end_main:
	subq %rsp, %rbp
	addq %rbp, %rsp
	popq %rbp
	ret
