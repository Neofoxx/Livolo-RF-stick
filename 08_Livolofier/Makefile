# Makefile for gcc for PIC32MX

# mipsel-elf-gcc & others should be in out PATH
# P32M_TOOLCHAIN_PATH can be provided by the PATH,
# or when running the job, ie. P32M_TOOLCHAIN_PATH=../.. make
# PIC32PROG_PATH is optional
TARGET = main
CC = mipsel-elf-gcc
COPY = mipsel-elf-objcopy
SIZE = mipsel-elf-size
ARCH = -march=m4k -EL -msoft-float  # m4k, endian little, no hardware floating-point
FAMILY = __PIC32MX__
MCU = __32MX270F256D__
CRYSTALFREQUENCY = 8000000L
MIN_HEAP_SIZE = _min_heap_size=2048

PIC32PROG_PATH = /opt/pic32/progyon_mz_enhancements/progyon


BUILD_DIR = build
C_SRC_DIR = src $(P32M_TOOLCHAIN_PATH)/include/support/interrupt src/drivers src/peripherals
INCLUDE_DIR = inc
ASM_DIR = src/asm

OPTIMIZATION = -O1

LDSCRIPTS = -T$(P32M_TOOLCHAIN_PATH)/proc/32MX270F256D/procdefs.ld -T$(P32M_TOOLCHAIN_PATH)/ldscripts/elf32pic32mx.ld
STARTUPSCRIPTS = $(P32M_TOOLCHAIN_PATH)/libpic32/startup/crt0.S $(P32M_TOOLCHAIN_PATH)/libpic32/startup/general-exception.S
INTERRUPTSCRIPT = $(P32M_TOOLCHAIN_PATH)/include/support/interrupt/interrupt.S	# The new isrwrapper/interrupt system
# Added build dir to LDFLAGS, so xc.h can find processor.o
LDFLAGS =  $(STARTUPSCRIPTS) $(INTERRUPTSCRIPT) $(LDSCRIPTS) -Wl,-undefined,dynamic_lookup -L build -lm -lc -lgcc -lnosys


INCLUDES = -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/drivers -I$(INCLUDE_DIR)/peripherals -I$(P32M_TOOLCHAIN_PATH)/include -I $(P32M_TOOLCHAIN_PATH)/include/support/interrupt # TODO paths
C_SOURCES = $(foreach dir, $(C_SRC_DIR), $(wildcard $(dir)/*.c)) 
ASM_SOURCES = $(foreach dir, $(ASM_DIR), $(wildcard $(dir)/*.s))
MCU_ASM = $(P32M_TOOLCHAIN_PATH)/proc/32MX270F256D/p32mx270f256d.S # TODO BETTER

# Get files in folders & subfolders, build to BUILD_DIR
OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJ += $(addprefix $(BUILD_DIR)/, $(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

CFLAGS = $(ARCH) -nostdlib $(OPTIMIZATION) -D $(FAMILY) -D $(MCU) \
 -Wl,-defsym,$(MIN_HEAP_SIZE) -Wl,-Map=$(BUILD_DIR)/output.map\
-Wall -ffunction-sections -fdata-sections -Wl,--gc-section -fdollars-in-identifiers #-Werror 
#

all: $(TARGET).hex
	
# Flash, debug print, skip verify (personal preference)
flash:
	$(PIC32PROG_PATH)/progyon $(BUILD_DIR)/$(TARGET).hex -D -S -s 8000

flash_icsp:
	$(PIC32PROG_PATH)/progyon $(BUILD_DIR)/$(TARGET).hex -D -S -i ICSP -s 8000


# For debug, recursively call the makefile again, with the DEBUG varible
# set to -g, so Debug symbols get built, and -D to define DEBUG_BUILD
# (useful to automate some cases with #ifdef later)
debug: 
	$(MAKE) $(MAKEFILE) DEBUG="-g -D DEBUG_BUILD"

clean: 
	rm -f $(BUILD_DIR)/*.hex
	rm -f $(BUILD_DIR)/*.elf
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.map
	rm -f $(BUILD_DIR)/*.s

# Make objects from C
# $< evaluates to FILE.c
# $@ evaluates to FILE.o 
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDES) -c $< -o $@

# Take for example p32mx250f128.s, and compile into processor.o (defines register locations)
# Input file should be p32mx ... .s (MCU_ASM)
# $@ evaluates to processor.o
processor.o:
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDES) -c $(MCU_ASM) -o $(BUILD_DIR)/$@

# target : prerequisites
# Compile .c files into .o, and processor.o (because name changes)
# Then link
$(TARGET).elf: $(OBJ) processor.o
	$(CC) $(CFLAGS) $(DEBUG) $(OBJ) $(BUILD_DIR)/processor.o $(LDFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$@

# Make .elf
# LINK
# Convert .elf to .hex
# Show used space :)
$(TARGET).hex: $(TARGET).elf
	$(COPY) -O ihex $(BUILD_DIR)/$< $(BUILD_DIR)/$(TARGET).hex
	$(SIZE) $(BUILD_DIR)/$(TARGET).elf

# Create build directory. 
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

