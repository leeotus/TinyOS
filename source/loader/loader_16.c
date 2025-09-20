__asm__(".code16gcc");

/**
 * @file loader_16.c
 * @author leeotus (leeotus@163.com)
 * @brief 16位实模式下的代码
 */

#include "loader.h"

boot_info_t boot_info;      // 启动参数信息

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
            show_msg("Failed to detect memory ...\r\n");
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


void loader_entry(void) {
    show_msg("Now loading ...\r\n");
    detect_memory();        // 检测内存容量
    for(;;) {

    }
}