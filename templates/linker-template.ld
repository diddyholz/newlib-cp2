ENTRY(_start);
STARTUP(crt0.o);

SECTIONS {
    start_address = 0x8CFE2000;
    . = start_address;
    .bootstrap.start : AT(start_address) {
        *(.bootstrap.start)
    }
    
    info_address = 0x8CFE2010;
    . = info_address;
    .hollyhock_name : {
        *(.hollyhock_name)
    }
    .hollyhock_description : {
        *(.hollyhock_description)
    }
    .hollyhock_author : {
        *(.hollyhock_author)
    }
    .hollyhock_version : {
        *(.hollyhock_version)
    }

    .bootstrap : {
        *(.bootstrap.text)
        *(.bootstrap.data)
    }

    .text : AT(text_address) {
        *(.text)
        *(.rodata*)
    }

    .data : {
        *(.data)
    }
    
    _edata = .;
    PROVIDE(edata = .);
    
    .bss : {
        *(.bss)
        *(COMMON)
    }
    
    _end = .;
    PROVIDE(end = .);

    .eh_frame : {
        *(.eh_frame*)
    }
}
