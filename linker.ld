ENTRY(start)
SECTIONS
{
    . = 1M; /* تحميل الكرنل عند العنوان 1 ميجابايت */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot) /* وضع توقيع الملتي بوت في البداية */
        *(.text)
    }
    .rodata BLOCK(4K) : ALIGN(4K) { *(.rodata) }
    .data BLOCK(4K) : ALIGN(4K)   { *(.data) }
    .bss  BLOCK(4K) : ALIGN(4K)   { *(COMMON) *(.bss) }
}
