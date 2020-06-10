        .globl  main
main:
pushq %rbp
movq %rsp, % rbp
movq $10, %rbx
pushq %rbx
movq -8(%rbp), %rbx
movq $2 , %rcx
addq %rcx, %rbx
pushq %rbx
movq -16(%rbp), %rbx
movq $3 , %rcx
addq %rcx, %rbx
pushq %rbx
movq -24(%rbp), %rcx
movq %rcx, %rbx
movq %rbx, -8(%rbp)