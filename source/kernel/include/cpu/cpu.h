#ifndef __CPU_H__
#define __CPU_H__

#include "comm/types.h"

// @file Segment Descriptor: assets/segment-descriptor.png
#pragma pack(1)
typedef struct _segment_desc_t {
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
} segment_desc_t;

#pragma pack()

/**
 * @brief 设置段描述符(Segment Descriptor)的函数, 主要用于x86架构下的内存管理
 * @param selector 段选择子, 用于定位GDT中的描述符
 * @param base 段的基地址(32位)
 * @param limit 段的界限(32位), 表示段的大小
 * @param attr 段的属性(16位), 包含访问权限, 类型等等
 * @note selector @file assets/segment-selector.png
 *
 */
void segment_desc_set(int selector, uint32_t base, uint32_t limit,
                      uint16_t attr);

// @brief 初始化GDT全局描述符表
void gdt_init(void);

void cpu_init(void);

#endif
