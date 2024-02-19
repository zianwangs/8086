CC = gcc
CCFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -fno-omit-frame-pointer
CCFLAGS += -no-pie -ffreestanding -nostdlib -nostdinc
AS = nasm
ASFLAGS = -f elf
LD = ld 
LDFLAGS = -m -static -nostdlib -N
OBJCOPY = objcopy
QEMU = qemu-system-x86_64
QEMU_FLAGS = -nographic

.PHONY: all clean

all: run

boot.bin: boot.asm load.c
	$(AS) $(ASFLAGS) -o boot.o boot.asm
	$(CC) $(CCFLAGS) -m32 -c load.c
	$(LD) $(LDFLAGS) -m elf_i386 -Ttext 0x7c00 -e entry -o boot boot.o load.o
	$(OBJCOPY) -S -O binary -j .text boot boot.bin

os.img: boot.bin boot.sig
	dd if=/dev/zero of=$@ count=2
	dd if=boot.bin of=$@ conv=notrunc
	dd if=boot.sig of=$@ obs=1 seek=510 conv=notrunc
	echo -n 'hello, world!' > ascii.text
	dd if=ascii.text of=$@ seek=1 conv=notrunc

run: os.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$<,index=0,format=raw,media=disk

clean:
	rm -f *.o *.bin *.text *.img *.out boot
