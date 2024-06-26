! file.s with os functions, imported from hollyhock sdk
.macro DEFINE_OS_FUNC name, addr
.globl _\name
.set _\name, \addr
.endm

DEFINE_OS_FUNC cas_close 0x80057912
DEFINE_OS_FUNC cas_findClose 0x8005A8BA
DEFINE_OS_FUNC cas_findNext 0x8005A5F0
DEFINE_OS_FUNC cas_findFirst 0x8005A2AC
DEFINE_OS_FUNC cas_fstat 0x8005798E
DEFINE_OS_FUNC cas_getAddr 0x80057C6A
DEFINE_OS_FUNC cas_lseek 0x80057A96
DEFINE_OS_FUNC cas_mkdir 0x80057814
DEFINE_OS_FUNC cas_open 0x80057854
DEFINE_OS_FUNC cas_read 0x800578A2
DEFINE_OS_FUNC cas_remove 0x8005794E
DEFINE_OS_FUNC cas_rename 0x80057A06
DEFINE_OS_FUNC cas_stat 0x800579C2
DEFINE_OS_FUNC cas_write 0x800578DA
