.code16
.text
.global _start
.extern loader_entry

_start:
	jmp loader_entry

	// 32位代码:
	.code32
	.text
	.global protected_mode_entry
	.extern load_kernel
protected_mode_entry:
	// 相当于函数, 声明写在"loader.h"中

	// 初始化寄存器
	mov $16, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	jmp $8, $load_kernel
	