.section .bootstrap.start, "x"
.global	_start
_start:
    ! init for hollyhock is only 16 bytes large
    ! therefore instantly jump to .text
    mov.l   start_address, r0
    jmp     @r0
    nop

! This should be long aligned, however the assembler would add a too large padding
.align 2 
start_address:
.long  actual_start
! load address for hollyhock launcher
load_address:
.long  _start

.section .bootstrap.text, "x"
actual_start:
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

    mov.l   cas_setup, r0
    jsr     @r0
    nop

    ! backup stack pointer and set return address for exit
    mov.l   sp_bak, r0
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

.global __exit_address
__exit_address:
    ! restore stack pointer
    mov.l   sp_bak, r8
    mov.l   @r8, r15

    mov.l   cas_cleanup, r0
    jsr     @r0
    nop

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
edata_k:
	.long	_edata
end_k:
	.long	_end
main_k:
	.long	_main
exit_k:
	.long	_exit
sp_bak:
    .long   _sp_bak
cas_setup:
    .long   _cas_setup
cas_cleanup:
    .long   _cas_cleanup
_sp_bak:
    .long   1
