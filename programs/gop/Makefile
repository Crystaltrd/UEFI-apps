ARCH = x86_64
TARGET = gop.efi
SRCS = $(wildcard *.c)

include uefi/Makefile

add: 
	mcopy -o -i ../../QEMU/fat.img $(TARGET) ::/
