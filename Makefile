AS = nasm
AS_FLAGS = -f bin
QEMU = qemu-system-x86_64
QEMU_FLAGS = -nographic

all: run

boot.bin: boot.asm
	$(AS) $(AS_FLAGS) -o $@ $^

os.img: boot.bin
	dd if=/dev/zero of=$@ count=2
	dd if=$< of=$@ conv=notrunc
	echo -n 'hello, world!' > ascii.text
	dd if=ascii.text of=$@ seek=1 conv=notrunc


run: os.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$<,index=0,format=raw,media=disk


.PHONY: clean

clean:
	rm -f *.o *.bin *.text *.img
