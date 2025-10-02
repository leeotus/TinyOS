    .text
    .extern kernel_init
    .global _start

    // _start视为"void start(boot_info_t *boot_info)"函数
_start:
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax

    push %eax

    // 调用kernel_init函数并且传递参数boot_info
    call kernel_init
    jmp .
