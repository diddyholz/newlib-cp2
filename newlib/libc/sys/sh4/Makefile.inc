if MAY_SUPPLY_SYSCALLS
libc_a_SOURCES += %D%/syscalls.c %D%/cas_file.S %D%/crt0.S 
endif
