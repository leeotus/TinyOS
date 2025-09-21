#include "loader.h"

/**
 * @brief 使用LBA模式读取磁盘
 * @param sector 要读取的扇区
 * @param sector_cnt 扇区个数
 * @param buffer 读取数据后存放的位置
 */
static void read_disk(uint32_t sector, uint32_t sector_cnt, uint8_t *buffer) {
    outb(0x1f6, 0xe0);
    outb(0x1f2, (uint8_t)(sector_cnt >> 8));
    outb(0x1f3, (uint8_t)(sector >> 24));
    outb(0x1f4, 0);
    outb(0x1f5, 0);

    outb(0x1f2, (uint8_t)(sector_cnt));
    outb(0x1f3, (uint8_t)(sector));
    outb(0x1f4, (uint8_t)(sector >> 8));
    outb(0x1f5, (uint8_t)(sector >> 16));

    outb(0x1f7, 0x24);

    uint16_t *data_buf = (uint16_t*)buffer;
    while(sector_cnt--) {
        while((inb(0x1f7) & 0x88) != 0x8);  // 未就绪，没有数据可读

        // 等待就绪,开始读取数据
        for(int i=0; i<SECTOR_SIZE/2; i++) {
            *data_buf++ = inw(0x1f0);
        }
    }
}

void load_kernel(void) {
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);
    for(;;) {

    }
}