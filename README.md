Hello World!

Build `TARGET=BOOTX64.EFI SRCS=helloworld.c make`
Run with `qemu-system-x86_64 -L /usr/share/edk2-ovmf/ -pflash /usr/share/edk2-ovmf/OVMF_CODE.fd -cdrom QEMU/fat.img -net none`


