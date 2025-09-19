#! /bin/bash
# 适用于Linux
export DISPLAY=localhost:10.0
qemu-system-i386 -daemonize -m 128M -s -S -drive file=disk1.img,index=0,media=disk,format=raw -drive file=disk2.img,index=1,media=disk,format=raw -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation -display sdl,gl=on -vga std -monitor tcp:127.0.0.1:4444,server,nowait
# qemu-system-i386 -daemonize -m 128M -s -S -drive file=disk1.img,index=0,media=disk,format=raw -drive file=disk2.img,index=1,media=disk,format=raw -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
