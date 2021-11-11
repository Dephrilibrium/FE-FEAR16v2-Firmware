##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.10.0-B14] date: [Mon Sep 14 16:48:01 CEST 2020]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = FEAR-16

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og
# silent compile (comment for full output!)
.SILENT:


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources ("manual" Scandepth 0-15, because make has no recursively scan)
C_SOURCES =  \
$(wildcard Sourcecode/*.c) \
$(wildcard Sourcecode/*/*.c) \
$(wildcard Sourcecode/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*.c) \
$(wildcard Sourcecode/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*.c)

# ASM sources
ASM_SOURCES =  \
$(wildcard TM4C123GH6PM/startup_tm4c.s) # Startup-file


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
DP = $(GCC_PATH)/$(PREFIX)objdump -h
SM = $(GCC_PATH)/$(PREFIX)nm
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
DP = $(PREFIX)objdump -h
SM = $(PREFIX)nm
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
# NONE for Cortex-M0/M0+/M3
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=softfp

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER


# AS includes
AS_INCLUDES = 

# C includes
BASE_DIR=./sourcecode
C_INCLUDES =  \
-I ./TM4C123GH6PM/CMSIS \
-I ./TM4C123GH6PM/system \
$(addprefix -I ,$(sort \
$(sort $(dir $(wildcard $(BASE_DIR)/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/*/*/*/*/*/))) \
$(sort $(dir $(wildcard $(BASE_DIR)/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/))) \
))



# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -Wno-unknown-pragmas -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = TM4C123GH6PM/TM4C123GH6PM.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -mthumb -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

echoInc:
	echo $(C_INCLUDES)

size:
	$(SZ) $(BUILD_DIR)/$(TARGET).elf

dump:
	$(DP) $(BUILD_DIR)/$(TARGET).elf

symbols:
	$(SM) $(BUILD_DIR)/$(TARGET).elf

plotinc:
	echo $(C_INCLUDES)

plotc:
	echo $(C_SOURCES)

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rmdir /q /s $(BUILD_DIR)

rebuild: clean all
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
