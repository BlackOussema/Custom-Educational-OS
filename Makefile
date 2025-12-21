# متغيرات المترجم
CC = gcc
AS = as
LD = ld
QEMU = qemu-system-i386

# خيارات المترجم (لضمان عمل الكرنل بشكل صحيح)
CFLAGS = -m32 -ffreestanding -O2 -fno-stack-protector -fno-pie -nostdlib
LDFLAGS = -m elf_i386 -T linker.ld

# الأهداف (Files)
ALL_FILES = boot.o kernel.o
BINARY = CyberShield.bin
DISK_IMG = CyberShield_Disk.img

# الأمر الرئيسي: سيقوم بكل شيء بالترتيب
all: clean build disk run

build:
	@echo "[+] Compiling Assembly..."
	$(AS) --32 boot.S -o boot.o
	@echo "[+] Compiling Kernel..."
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o
	@echo "[+] Linking everything into $(BINARY)..."
	$(LD) $(LDFLAGS) -o $(BINARY) $(ALL_FILES)

disk:
	@echo "[+] Creating Virtual Hard Disk..."
	@if [ ! -f $(DISK_IMG) ]; then qemu-img create $(DISK_IMG) 10M; fi

run:
	@echo "[+] Starting CyberShieldTN in QEMU..."
	$(QEMU) -kernel $(BINARY) -hda $(DISK_IMG)

clean:
	@echo "[+] Cleaning old files..."
	rm -f *.o $(BINARY)
