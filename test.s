	.file	"cctest.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
    addi sp, sp, -50
    addi a2, a2, 6

    add a2, a2, sp
    sub a2, a2, sp
    sll a3, a2, sp
    slt a4, a2, sp
    sltu a5, a2, sp
    xor a1, a2, sp
    srl a2, a2, sp
    sra a3, a2, sp
    or a4, a2, sp
    and a5, a2, sp

	
	.size	main, .-main
	.ident	"GCC: (GNU) 10.2.0"
