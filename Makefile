CC = gcc
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS += -no-pie -ffreestanding -nostdlib -nostdinc
AS = nasm
ASFLAGS = -f elf64
LD = ld 
LDFLAGS = -static -nostdlib -N
OBJCOPY = objcopy
QEMU = qemu-system-x86_64
QEMU_FLAGS = -nographic -d int -no-reboot

.PHONY: all clean

all: run

OBJS = \
	main.o \

boot.bin: boot.asm load.c
	$(AS) $(ASFLAGS) -o boot.o boot.asm
	$(CC) $(CFLAGS) -c load.c
	$(LD) $(LDFLAGS) -Ttext 0x7c00 -e entry -o boot boot.o load.o
	$(OBJCOPY) -S -O binary -j .text boot $@

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -Ttext 0x80000000 -e main -o main $^
	$(OBJCOPY) -S -O binary -j .text main $@

os.img: boot.bin boot.sig kernel.bin
	dd if=/dev/zero of=$@ count=2
	dd if=boot.bin of=$@ conv=notrunc
	dd if=boot.sig of=$@ obs=1 seek=510 conv=notrunc
	dd if=kernel.bin of=$@ seek=1 conv=notrunc

run: os.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$<,index=0,format=raw,media=disk

clean:
	rm -f *.o *.bin *.text *.img *.out boot main
