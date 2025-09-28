#ifndef __BOOT_INFO_H__
#define __BOOT_INFO_H__

#include "types.h"

#define BOOT_RAM_REGION_MAX     10
#define SECTOR_SIZE             512
#define SYS_KERNEL_LOAD_ADDR    (1024 * 1024)       // ELF文件存放的内存地址位置

typedef struct _boot_info_s boot_info_t;

// @brief 启动信息参数
struct _boot_info_s {

    // RAM区信息
    struct {
        uint32_t start;
        uint32_t size;
    } ram_region_cfg[BOOT_RAM_REGION_MAX];

    int ram_region_cnt;         // 记录ram_region_cfg保存的信息个数
};

#endif
