__asm__(".code16gcc");

/**
 * @file loader_16.c
 * @author leeotus (leeotus@163.com)
 * @brief 16位实模式下的代码
 */

#include "loader.h"

boot_info_t boot_info;      // 启动参数信息

// GDT表
uint16_t gdt_table[][4] = {
    {0, 0, 0, 0},
    {0xffff, 0x0000, 0x9a00, 0x00cf},
    {0xffff, 0x0000, 0x9200, 0x00cf},
};

/**
 * @brief 用于在屏幕上显示字符串
 * @param msg 需要打印的字符串数据 
 * @ref https://blog.csdn.net/m0_50662680/article/details/130875689
 */
static void show_msg(const char *msg) {
    char c;
    // while((c = *msg++) != '\0') {
    //     __asm__ __volatile__ (
    //         "mov $0xe, %%ah\n\t"
    //         "mov %[ch], %%al\n\t"
    //         "int $0x10"::[ch]"r"(c)
    //     );
    // }
    // or:
    while((c = *msg++) != '\0') {
        __asm__ __volatile__ (
            "mov $0xe, %%ah\n\t"
            "mov %0, %%al\n\t"
            "int $0x10"
            :
            :"r"(c)
        );
    }
}

/**
 * @brief 检测系统的内存容量
 * @ref https://wiki.osdev.org/Detecting_Memory_(x86)
 * @ref https://www.zshanjun.com/discussions/250
 */
static void detect_memory(void) {
    int i=0;
    SMAP_entry_t smap_entry;
    SMAP_entry_t *entry = &smap_entry;
    uint32_t countID = 0;
    int signature, bytes;

    show_msg("Detecting memory ...\r\n");
    for(i=0; i<BOOT_RAM_REGION_MAX; ++i) {
        // NOTE "D"(entry)表示把entry的地址放入到edi寄存器中，作为缓存区指针
        __asm__ __volatile__ (
            "int $0x15"
            : "=a"(signature), "=c"(bytes), "=b"(countID)
            : "a"(0xe820), "b"(countID), "c"(24), "d"(0x534d4150), "D"(entry)
        );
        if(signature != 0x534d4150) {
            // 检测内存失败
            show_msg("Failed to detect memory ... ");
            return;
        }
        if(bytes > 20 && (entry->ACPI & 0x0001) == 0) {
            // 忽略
            continue;
        }
        if(entry->Type == 1) {
            boot_info.ram_region_cfg[boot_info.ram_region_cnt].start = entry->BaseL;
            boot_info.ram_region_cfg[boot_info.ram_region_cnt].size = entry->LengthL;
            boot_info.ram_region_cnt += 1;
        }
        if(countID == 0) {
            // 读取完毕
            break;
        }
    }
    show_msg("ok.\r\n");
}

/**
 * @brief 从实模式切换到保护模式
 */
static void enter_protected_mode(void) {
    cli();  // 关中断
    uint8_t v = inb(0x92);
    outb(0x92, v | 0x2);

    // 加载GDT
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));

    uint32_t cr0 = read_cr0();
    write_cr0(cr0 | (1 << 0));

    // 远跳转指令
    far_jump(8, (uint32_t)protected_mode_entry);
}

void loader_entry(void) {
    show_msg("Now loading ...\r\n");
    detect_memory();        // 检测内存容量
    enter_protected_mode(); // 进入保护模式
    for(;;) {

    }
}