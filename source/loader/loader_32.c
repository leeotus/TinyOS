#include "loader.h"
#include "comm/elf.h"

/**
 * @brief 使用LBA模式读取磁盘
 * @param sector 要读取的扇区
 * @param sector_cnt 扇区个数
 * @param buffer 读取数据后存放的位置
 * @note sector_cnt应该设置得足够大以保证可以把内核代码全部从磁盘中读取出来
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

    uint16_t *data_buf = (uint16_t *)buffer;
    while (sector_cnt--) {
        while ((inb(0x1f7) & 0x88) != 0x8)
            ; // 未就绪，没有数据可读

        // 等待就绪,开始读取数据
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            *data_buf++ = inw(0x1f0);
        }
    }
}

/**
 * @brief 解析ELF文件
 * @param file_buffer elf文件所在的内存地址
 */
static uint32_t reload_elf_file(uint8_t *file_buffer) {
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr *)file_buffer;
    if ((elf_hdr->e_ident[0] != 0x7f) || (elf_hdr->e_ident[1] != 'E') ||
        (elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F')) {
        return 0;
    }

    for (int i = 0; i < elf_hdr->e_phnum; ++i) {
        Elf32_Phdr *phdr = (Elf32_Phdr *)(file_buffer + elf_hdr->e_phoff) + i;
        if(phdr->p_type != PT_LOAD) {
            continue;
        }

        uint8_t *src = file_buffer + phdr->p_offset;
        uint8_t *dst = (uint8_t *)phdr->p_paddr;
        for(int j = 0; j < phdr->p_filesz; ++j) {
            *dst++ = *src++;
        }
        dst = (uint8_t *)phdr->p_paddr + phdr->p_filesz;
        for(int j = 0; j < phdr->p_memsz - phdr->p_filesz; ++j) {
            *dst++ = 0;
        }
    }
    return elf_hdr->e_entry;
}

// TODO
static void die(int code) {
    for (;;) {
    }
}

void load_kernel(void) {
    // 将kernel.elf(放在磁盘的第100个扇区处)读取到内存的SYS_KERNEL_LOAD_ADDR处
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);

    uint32_t kernel_entry = reload_elf_file((uint8_t *)SYS_KERNEL_LOAD_ADDR);
    if(kernel_entry == 0) {
        // error:
        die(-1);
    }

    ((void (*)(boot_info_t *))kernel_entry)(&boot_info);
    for(;;) {

    }
}
