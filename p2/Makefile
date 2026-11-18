TARGET := interrupts

PREFIX := arm-none-eabi-

CC      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
SIZE    := $(PREFIX)size

CPUFLAGS := -mcpu=arm7tdmi -marm

CFLAGS := \
	$(CPUFLAGS) \
	-std=c11 \
	-Og \
	-g3 \
	-ffreestanding \
	-fdata-sections \
	-ffunction-sections \
	-Wall \
	-Wextra

ASFLAGS := \
	$(CPUFLAGS) \
	-g3

LDFLAGS := \
	$(CPUFLAGS) \
	-T linker.ld \
	-nostdlib \
	-Wl,--gc-sections \
	-Wl,-Map=$(TARGET).map

OBJS := startup.o uart.o interrupts.o


all: $(TARGET).elf $(TARGET).bin

startup.o: startup.S
	$(CC) $(ASFLAGS) -c $< -o $@

uart.o: uart.c
	$(CC) $(CFLAGS) -c $< -o $@

interrupts.o: interrupts.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

dump: $(TARGET).elf
	$(OBJDUMP) -d -S $<

clean:
	rm -f *.o *.elf *.bin *.map

flash:
	openocd -f interface/cmsis-dap -f mcb2300.cfg -c 'init' -c 'reset halt'   -c 'program $(TARGET).elf' -c 'reset run' -c 'shutdown'

.PHONY: all clean dump
