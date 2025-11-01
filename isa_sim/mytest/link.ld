ENTRY(_start)

SECTIONS {
  . = 0x2000;

  .text   : { *(.text*) }
  .rodata : { *(.rodata*) }
  .data   : { *(.data*) }

  /* 撐出實際 RAM：收完 .bss 後再加一段零初始化空間給 stack */
  .bss : {
    *(.bss*) *(COMMON)
    . = ALIGN(16);
    __bss_end = .;
    . = . + 0x40000;       /* 256 KB RAM */
    . = ALIGN(16);
    _stack_top = . - 256;  /* 預留 256B，避免 prologue 正偏移越界 */
  }
}
