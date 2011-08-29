###############################################################################
# Makefile for the research project
###############################################################################

## General Flags
PROJECT = main
TARGET = build/main.elf
CC = msp430-gcc


MCU = msp430g2231


## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

override CFLAGS = -g -Wall -Os -mmcu=$(MCU) $(DEFS) -gdwarf-2


## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(TARGET:%.elf=%.map),--relax


## Objects explicitly added by the user
LINKONLYOBJECTS = 

SRC_DIR   := src/
BUILD_DIR := build/

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ       := $(patsubst src/%.c,build/%.o,$(SRC))
DEP		  := $(OBJ:%.o=%.d)
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.c $(SRC_DIR)

all: checkdirs $(TARGET) size


-include $(DEP)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf build/
#	@rm -f *.hex
#	@rm -f *.elf
#	@rm -f *.lss
#	@rm -f *.map
#	@rm -f main.txt
	
define make-goal
$1/%.o: %.c
	@echo "CC" $$< " > " $$@
	@$(CC) $(INCLUDES) $(CFLAGS) -c $$< -MD -o $$@
endef

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

$(TARGET): $(OBJ)
	@echo "====================="
	@echo "  CC  >" $@
	@$(CC) $(LDFLAGS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) $^ -o $@
	@echo "====================="

%.lss: $(TARGET)
	msp430-objdump -h -S $< > $@

size: $(TARGET)
	@echo
	@msp430-size ${TARGET}

prog: 
	mspdebug rf2500 'prog $(TARGET)'
