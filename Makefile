all:
	cd lab7/kernel && make
	cd lab7/bootloader && make
	cd lab7/program && make
#	cd rootfs && (find . | cpio -o -H newc > ../initramfs.cpio)

.PHONY: clean
clean:
	cd lab4/kernel && make clean
	cd lab4/bootloader && make clean
	cd lab4/program && make clean
	cd lab6/kernel && make clean
	cd lab6/bootloader && make clean
	cd lab6/program && make clean
	cd lab7/kernel && make clean
	cd lab7/bootloader && make clean
	cd lab7/program && make clean

.PHONY: run
run:
	cd lab7/kernel && make run

.PHONY: lab4
lab4:
	cd lab4/kernel && make run

.PHONY: lab6
lab6:
	cd lab6/kernel && make run_display

.PHONY: lab7
run_display:
	cd lab7/kernel && make run_display

.PHONY: bl
bl:
	cd lab7/bootloader && make

.PHONY: rootfs
rootfs:
	cd lab7/rootfs && ( ls -1 | cpio -o -H newc > ../initramfs.cpio)