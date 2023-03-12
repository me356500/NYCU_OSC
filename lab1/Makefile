OS := $(shell uname)

ifeq ($(OS), Linux)
	ARMGNU  = aarch64-linux-gnu
	MACHINE = raspi3b
endif

ifeq ($(OS), Darwin)
	ARMGNU  = aarch64-unknown-linux-gnu
	MACHINE = raspi3b
endif

CC     = $(ARMGNU)-gcc
LK     = $(ARMGNU)-ld
OBJCPY = $(ARMGNU)-objcopy
QEMU   = qemu-system-aarch64

A_SRCS = $(wildcard kernel/lib/*.S)
C_SRCS = $(wildcard kernel/lib/*.c)
SRC_DIR = kernel/lib
OBJ_DIR = kernel/build
OBJS = $(A_SRCS:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.o) $(C_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INCLUDE = kernel/include
CFLAGS = -ggdb -Wno-implicit -ffreestanding -nostdlib -nostartfiles -I$(INCLUDE)

IMAGE = kernel8

all: $(IMAGE).img 

$(IMAGE).img: $(IMAGE).elf
	$(OBJCPY) -O binary $(IMAGE).elf kernel8.img

$(IMAGE).elf: $(OBJS)
	$(LK) $(OBJS) -T kernel/linker.ld -o $(IMAGE).elf

$(OBJ_DIR)/%.o: kernel/lib/%.S
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJ_DIR)/%.o: kernel/lib/%.c
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
	$(CC) -c $< $(CFLAGS) -o $@

run: $(IMAGE).img 
	$(QEMU) -machine $(MACHINE) -kernel $(IMAGE).img -display none -serial null -serial stdio

dbg: $(IMAGE).img 
	$(QEMU) -machine $(MACHINE) -kernel $(IMAGE).img -display none -serial null -serial stdio -s -S

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) *.img *.elf */*.elf