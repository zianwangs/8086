CC = gcc
CFLAGS = -fno-builtin -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS += -ffreestanding -nostdlib -nostdinc -fno-pie -mcmodel=large
AS = nasm
ASFLAGS = -f elf64
LD = ld 
LDFLAGS = -nostdlib -static
OBJCOPY = objcopy
QEMU = qemu-system-x86_64
QEMU_FLAGS = -nographic  -no-reboot -smp 1

KERNEL_BLOCKS = $(shell stat -c%b kernel.bin)

.PRECIOUS: %.o

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
	proc.o \
	console.o \
	vm.o \
	kalloc.o \
	print.o \
	swtch.o \
	util.o \
	crt1.o \


vector.asm: vector.py 
	python3 $^ > $@

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $^

%.bin: %.o user.ld crt1.o
	$(LD) $(LDFLAGS) -T user.ld -o $@ crt1.o $< 

boot.o: boot.asm
	$(AS) $(ASFLAGS) -dKERNEL_BLOCKS=$(KERNEL_BLOCKS) -o $@ $^

boot.bin: boot.o load.o
	$(LD) $(LDFLAGS) -N -Ttext 0x7c00 -e entry -o boot $^
	$(OBJCOPY) -S -O binary -j .text boot $@
	
kernel.bin: $(OBJS) kernel.ld init.bin Makefile
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(OBJS) -b binary init.bin

os.img: kernel.bin boot.bin boot.sig Makefile
	dd if=/dev/zero of=$@ count=$$(( 1 + $(KERNEL_BLOCKS) ))
	dd if=boot.bin of=$@ conv=notrunc
	dd if=boot.sig of=$@ obs=1 seek=510 conv=notrunc
	dd if=kernel.bin of=$@ seek=1 conv=notrunc

run: os.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$<,index=0,format=raw,media=disk

debug: os.img
	$(QEMU) $(QEMU_FLAGS) -d int -drive file=$<,index=0,format=raw,media=disk

clean:
	rm -f *.o *.bin *.text *.img *.out boot
