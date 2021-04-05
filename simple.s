	.file	"simple.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48    ; sp = sp = sp -48;
	sw	s0,44(sp)    ; mem[ sp + 44] = s0
	addi	s0,sp,48     ; s0 = sp + 48
	sw	a0,-36(s0)   ; mem[s0 - 36] = a0
	sw	a1,-40(s0)   ; mem[s0 - 40] = a1
	sw	zero,-20(s0) ; mem[s0 - 20] = zero
	sw	zero,-24(s0) ; mem[s0 -24] = zero
	j	.L2          ; jump L2
.L3:
	lw	a4,-20(s0) ; a4 = mem[s0 -20]
	lw	a5,-24(s0) ; a5 = mem[s0 -24]
	add	a5,a4,a5   ; a5 = a4 + a5 (a5 = 0)
	sw	a5,-20(s0) ; mem[s0 -20] = a5 
	lw	a5,-24(s0) ; a5 = mem[s0 - 24]
	addi	a5,a5,1    ; a5 = a5 + 1
	sw	a5,-24(s0) ; mem[s0 -24] = a5
.L2:
	lw	a4,-24(s0) ; a4 = mem[s0 - 24]
	li	a5,14      ; a5 = 14
	ble	a4,a5,.L3  ; if a4 < a5 jump to L3
	li	a5,0
	mv	a0,a5
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 10.2.0"
