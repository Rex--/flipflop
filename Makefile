-include flipflop.cfg
################################################################################
#  |----------|                                                                #
#  | flipflop |                                                                #
#  |----------|                                                                #
################################################################################
#    Project Configuration    #

TARGET := flipflop

## The device to compile for
MCU := 16LF19196

## Source code directories (separated by a space)
SRC_DIR := src

## Include directories (separated by a space)
INC_DIR := include

## Build files directory
BUILD_DIR := build

################################################################################
#    Compiler Setup   #

CC := xc8-cc

TARGET_ARCH := -mcpu=$(MCU)#				# Target Arch flag
CFLAGS := -O2 $(addprefix -I,$(INC_DIR))#	# Options for the compiler
# Linker options
LFLAGS := -Wl,-Map=${BUILD_DIR}/${TARGET}.map -mrom=0x0-0x3FF
# Firmware configuration options
FWFLAGS := -DPROG_OFFSET=$(PROG_OFFSET) -DBOOTLOADER_PIN=$(BOOTLOADER_EN) \
	-DUART_RX_PPS_VAL=$(BOOTLOADER_RX) -DUART_TX_PPS_REG=$(BOOTLOADER_TX)

################################################################################
#    Match n' Making    #

# Find all .c files in source directories
SOURCES := $(foreach srcdir,$(SRC_DIR),$(wildcard $(srcdir)/*.c))
# Find all .S files 
ASOURCES := $(foreach srcdir,$(SRC_DIR),$(wildcard $(srcdir)/*.S))
# Generate list of p-code files from C sources
OBJECTS := $(SOURCES:%=$(BUILD_DIR)/%.p1)
# Generate list of assembly object files
AOBJECTS := $(ASOURCES:%=$(BUILD_DIR)/%.o)
# Generate list of depends files from objects (pcode)
DEPENDS := $(OBJECTS:%.p1=%.d)

# Generate p-code object files from C source files
$(BUILD_DIR)/%.c.p1: %.c Makefile flipflop.cfg
	@mkdir -p $(dir $@)
	$(CC) $(TARGET_ARCH) $(CFLAGS) $(FWFLAGS) -c $< -o $@

# Generate object files from assembly source files
$(BUILD_DIR)/%.S.o: %.S Makefile flipflop.cfg
	@mkdir -p $(dir $@)
	$(CC) $(TARGET_ARCH) $(CFLAGS) $(FWFLAGS) -c $< -o $@

# Generate bin files (.hex and .elf)
$(BUILD_DIR)/$(TARGET).hex: $(OBJECTS) $(AOBJECTS) Makefile
	$(CC) $(TARGET_ARCH) $(CFLAGS) ${LFLAGS} $(OBJECTS) $(AOBJECTS) -o $@

.PHONY: all info clean fclean

all: $(BUILD_DIR)/$(TARGET).hex

clean:
	rm -f $(OBJECTS)
	rm -f $(AOBJECTS)
	rm -f $(DEPENDS)
	rm -f $(BUILD_DIR)/$(TARGET).*

fclean:
	rm -rf build

info:
	@echo
	@echo "Name:" $(TARGET)
	@echo "Source files:" $(SOURCES)
	@echo "Assembly files:" $(ASOURCES)
	@echo "Object files:" $(OBJECTS) $(AOBJECTS)
	@echo "Build directory:" $(BUILD_DIR)/
	@echo "Target Arch:" $(MCU)

# Include dependency files
-include $(DEPENDS)
