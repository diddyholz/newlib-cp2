.macro DEFINE_OS_FUNC name, addr
.globl _\name
.set _\name, \addr
.endm

DEFINE_OS_FUNC free 0x800A76FC
DEFINE_OS_FUNC malloc 0x800CFB00
