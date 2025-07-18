ARCH = x86_64
TARGET = tetris.efi
SRCS = $(wildcard *.c)
LIBS = -lm
include uefi/Makefile

format: 
	clang-format -i $(SRCS)
test: 
	cp $(TARGET) ../../QEMU/storage/
	qemu-system-x86_64 -bios ../../QEMU/OVMF_CODE.fd  -hda fat:rw:../../QEMU/storage -net none -monitor stdio -audiodev pa,id=speaker -machine pcspk-audiodev=speaker
