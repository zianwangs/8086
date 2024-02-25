CC = gcc
CFLAGS = -fno-builtin -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS += -ffreestanding -nostdlib -nostdinc
AS = nasm
ASFLAGS = -f elf64
LD = ld 
LDFLAGS = -nostdlib -static
OBJCOPY = objcopy
QEMU = qemu-system-x86_64
QEMU_FLAGS = -nographic  -no-reboot -smp 1

.PHONY: all clean

all: run

OBJS = \
	main.o \
	ioapic.o \
	lapic.o \
	pic.o \
	uart.o \
	trampoline.o \
	vector.o \
	trap.o \
	print.o \

boot.bin: boot.asm load.c
	$(AS) $(ASFLAGS) -o boot.o boot.asm
	$(CC) $(CFLAGS) -c load.c
	$(LD) $(LDFLAGS) -N -Ttext 0x7c00 -e entry -o boot boot.o load.o
	$(OBJCOPY) -S -O binary -j .text boot $@

vector.o: vector.py 
	python3 $^ > vector.asm
	$(AS) $(ASFLAGS) -o $@ vector.asm

trampoline.o: trampoline.asm
	$(AS) $(ASFLAGS) -o $@ $^
	
kernel.bin: $(OBJS) kernel.ld Makefile
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(OBJS)

os.img: boot.bin boot.sig kernel.bin Makefile
	dd if=/dev/zero of=$@ count=17
	dd if=boot.bin of=$@ conv=notrunc
	dd if=boot.sig of=$@ obs=1 seek=510 conv=notrunc
	dd if=kernel.bin of=$@ seek=1 conv=notrunc

run: os.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$<,index=0,format=raw,media=disk

clean:
	rm -f *.o *.bin *.text *.img *.out boot
