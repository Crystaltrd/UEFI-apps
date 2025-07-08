ARCH = x86_64
TARGET = helloworld.efi
SRCS = $(wildcard *.c)

include uefi/Makefile

add: 
	mcopy -o -i ../../QEMU/fat.img $(TARGET) ::/
