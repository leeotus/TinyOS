// ████████╗██╗███╗   ██╗██╗   ██╗ ██████╗ ███████╗
// ╚══██╔══╝██║████╗  ██║╚██╗ ██╔╝██╔═══██╗██╔════╝
//    ██║   ██║██╔██╗ ██║ ╚████╔╝ ██║   ██║███████╗
//    ██║   ██║██║╚██╗██║  ╚██╔╝  ██║   ██║╚════██║
//    ██║   ██║██║ ╚████║   ██║   ╚██████╔╝███████║
//    ╚═╝   ╚═╝╚═╝  ╚═══╝   ╚═╝    ╚═════╝ ╚══════╝
/**
 * 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 之后，将由BIOS跳转至0x7c00处开始运行
 */
	#include "boot.h"

  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern boot_entry
_start:
	// 初始化:
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs

	mov $_start, %esp		// 设置栈顶,@NOTE: 这里简单设置栈顶为0x7c00
							// 相当于: mov $0x7c00, %esp

read_loader:
	mov $0x8000, %bx		// 指定loader存放的位置
	mov $0x2, %ah			// AH=02H
	mov $0x2, %cx			// CH=柱面, CL=扇区
	mov $64	, %al			// AL=要读取的扇区数
	mov $0x0080, %dx		// DH=磁头, DL=驱动器 80H~0FFH:硬盘
	int $0x13				// 读取数据存放到指定位置
	// 结果: CF=0:操作成功, AH=00H,AL=传输的扇区数, 否则AH=状态码
	jc read_loader			// 发生错误的时候重试

	jmp boot_entry
	jmp .
	
	// 引导结束段
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
