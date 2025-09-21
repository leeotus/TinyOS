#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "types.h"

// @brief 关中断指令
static inline void cli(void) {
    __asm__ __volatile__ ("cli");
}

// @brief 开启中断
static inline void sti(void) {
    __asm__ __volatile__ ("sti");
}

/**
 * @brief 读取8位数据
 * @param port port 表示从哪个端口读出
 * @return uint8_t 读取的数据
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t rv = 0;
    // inb al, dx
    __asm__ __volatile__ (
        "inb %1, %0"
        : "=a"(rv)
        : "d"(port)
    );
    return rv;
}

// @brief 类似inb函数,但是读取的是16位大小
static inline uint16_t inw(uint16_t port) {
    uint16_t rv = 0;
    // in al, dx
    __asm__ __volatile__ (
        "in %1, %0"
        : "=a"(rv)
        : "d"(port)
    );
    return rv;
}

/**
 * @brief 输出8位数据到指定端口
 * @param port 输出的端口
 * @param data 要输出的数据
 */
static inline void outb(uint16_t port, uint8_t data) {
    // outb al, dx (outb data, port)
    __asm__ __volatile__ (
        "outb %0, %1"
        :
        : "a"(data), "d"(port)
    );
}

// @brief 加载GDT
static inline void lgdt(uint32_t start, uint32_t size) {
    struct {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    } gdt;

    gdt.start31_16 = start >> 16;
    gdt.start15_0 = start & 0xffff;
    gdt.limit = size - 1;

    // NOTE "m"表示memory
    __asm__ __volatile__ (
        "lgdt %[g]"
        :
        : [g]"m"(gdt)
    );
}

// @brief 读取控制寄存器cr0的值
static inline uint32_t read_cr0(void) {
    uint32_t cr0;

    __asm__ __volatile__ (
        "mov %%cr0, %[v]"
        : [v]"=r"(cr0)
        :
    );

    return cr0;
}

static inline void write_cr0(uint32_t v) {
    __asm__ __volatile__ (
        "mov %[v], %%cr0"
        :
        : [v]"r"(v)
    );
}

/**
 * @brief 远跳转
 * @param selector 段选择子
 * @param offset 跳转的地址
 */
static inline void far_jump(uint32_t selector, uint32_t offset) {
    uint32_t addr[] = {offset, selector};

    /**
     * @brief ljmp是远跳转指令，它会同时改变CS(代码段寄存器)和EIP(指令指针)
     * 实现从实模式切换到保护模式
     * @ref 有关“段描述符、选择子”的参考: https://akaha.blog.csdn.net/article/details/137545346?spm=1001.2014.3001.5502
     * @ref 从实模式到保护模式: https://blog.csdn.net/qq_41884002/article/details/138671148
     */
    __asm__ __volatile__ (
        "ljmpl *(%[a])"
        :
        :[a]"r"(addr)
    );
}

#endif