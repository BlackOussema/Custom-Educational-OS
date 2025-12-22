.set MAGIC, 0x1BADB002
.set FLAGS, 0x01
.set CHECKSUM, -(0x1BADB002 + 0x01)

.section .multiboot
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.global start        
start:               
    mov $stack_top, %esp
    call kernel_main
    cli
loop: hlt
    jmp loop

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:
