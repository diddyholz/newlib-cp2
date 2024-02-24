.section .init
.global	start
start:
    ! backup registers
    mov.l   r13, @-r15
    mov.l   r12, @-r15
    mov.l   r11, @-r15
    mov.l   r10, @-r15
    mov.l   r9, @-r15
    mov.l   r8, @-r15
	sts.l   pr, @-r15

	! zero out bss
	mov.l	edata_k,r0
	mov.l	end_k,r1
	mov	    #0, r2
start_l:
	mov.l	r2, @r0
	add	    #4, r0
	cmp/ge	r0,r1
	bt	    start_l

    ! backup stack pointer and set return address for exit
    mova    sp_bak, r0
    mov.l   r15, @r0

	! call main
	mov.l	main_k, r0
	jsr	    @r0
	nop

	! call exit
	mov	    r0,r4
	mov.l   exit_k,r0
	jsr	    @r0
	nop

.global exit_address
exit_address:
    ! restore stack pointer
    mov.l   sp_bak, r15

    ! restore registers
    lds.l   @r15+, pr
    mov.l   @r15+, r8
    mov.l   @r15+, r9
    mov.l   @r15+, r10
    mov.l   @r15+, r11
    mov.l   @r15+, r12
    mov.l   @r15+, r13

    rts
    nop

.align 4
sp_bak:
    .long   0
edata_k:
	.long	_edata
end_k:
	.long	_end
main_k:
	.long	_main
exit_k:
	.long	_exit