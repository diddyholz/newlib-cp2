libc_a_SOURCES += %D%/malloc.S %D%/malloc_r.c %D%/memcpy.S %D%/memset.S %D%/setjmp.S %D%/strcpy.S %D%/strlen.S %D%/strcmp.S

if SH64
libc_a_SOURCES += %D%/strncpy.S
endif
