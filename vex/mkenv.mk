# VEXcode mkenv.mk 2022_06_26_01

# Macros to help with paths that include spaces
sp = $() $()
qs = $(subst ?, ,$1)
sq = $(subst $(sp),?,$1)

# Default platform and build location
PLATFORM  = vexv5
BUILD     = build

# Version for clang headers
HEADERS = 18

# Project name passed from app
PROJECT  := $(if $(P),$(P),$(call qs,$(notdir $(call sq,${CURDIR}))))

# Check if the PROJECT name contains any whitespace
ifneq (1,$(words $(PROJECT)))
$(error Project name cannot contain whitespace: $(PROJECT))
endif

# SDK path passed from app
VEX_SDK_PATH ?= $(if $(T),$(T),${HOME}/sdk)

# printf_float flag name passed from app (not used in this version)
ifeq ("$(origin PRINTF_FLOAT)", "command line")
PRINTF_FLAG = -u_printf_float
endif

# Verbose flag passed from app
BUILD_VERBOSE ?= $(if $(V),$(V),$(if $(VERBOSE),$(VERBOSE),0))

# Use verbose flag
Q = $(if $(filter 0,$(BUILD_VERBOSE)),@,)

# Compile and link tools
CC      = clang
CXX     = clang
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size
LINK    = arm-none-eabi-ld
ARCH    = arm-none-eabi-ar
ECHO    = @echo
DEFINES = -DVexV5

# Platform specific macros
ifeq ($(OS),Windows_NT)
$(info windows build for platform $(PLATFORM))
SHELL = cmd.exe
MKDIR = md "$(@D)" 2> nul || :
RMDIR = rmdir /S /Q
CLEAN = $(RMDIR) $(BUILD) 2> nul || :
else
# Which flavor of linux
UNAME := $(shell sh -c 'uname -sm 2>/dev/null || Unknown')
$(info unix build for platform $(PLATFORM) on $(UNAME))
MKDIR = mkdir -p "$(@D)" 2> /dev/null || :
RMDIR = rm -rf
CLEAN = $(RMDIR) $(BUILD) 2> /dev/null || :
endif

# Toolchain include and lib locations
TOOL_INC  = -I"$(VEX_SDK_PATH)/$(PLATFORM)/clang/$(HEADERS)/include" \
            -I"$(VEX_SDK_PATH)/$(PLATFORM)/gcc/include/c++/13.3.1" \
            -I"$(VEX_SDK_PATH)/$(PLATFORM)/gcc/include/c++/13.3.1/arm-none-eabi/thumb/v7-a+fp/softfp" \
            -I"$(VEX_SDK_PATH)/$(PLATFORM)/gcc/include"

TOOL_LIB  = -L"$(VEX_SDK_PATH)/$(PLATFORM)/gcc/libs"

# Compiler flags
CFLAGS_CL = -target thumbv7-none-eabi -fshort-enums -Wno-unknown-attributes -U__INT32_TYPE__ -U__UINT32_TYPE__ -D__INT32_TYPE__=long -D__UINT32_TYPE__='unsigned long'
CFLAGS_V7 = -march=armv7-a -mfpu=neon -mfloat-abi=softfp
CFLAGS    = ${CFLAGS_CL} ${CFLAGS_V7} -O3 -Wall -Wextra -Werror=return-type -std=gnu23 $(DEFINES)
CXX_FLAGS = ${CFLAGS_CL} ${CFLAGS_V7} -O3 -Wall -Wextra -Werror=return-type -fno-rtti -fno-threadsafe-statics -std=gnu++23 -ffunction-sections -fdata-sections $(DEFINES)

# Linker flags
LNK_FLAGS = -z noexecstack --no-warn-rwx-segments -nostdlib -T "$(VEX_SDK_PATH)/$(PLATFORM)/lscript.ld" -R "$(VEX_SDK_PATH)/$(PLATFORM)/stdlib_0.lib" -Map="$(BUILD)/$(PROJECT).map" --gc-section -L"$(VEX_SDK_PATH)/$(PLATFORM)" ${TOOL_LIB}

# Future static library
PROJECTLIB = lib$(PROJECT)
ARCH_FLAGS = rcs

# Libraries
LIBS =  --start-group -lv5rt -lstdc++ -lc -lm -lgcc --end-group

# Include file paths
INC += $(addprefix -I, ${INC_F})
INC += -I"$(VEX_SDK_PATH)/$(PLATFORM)/include"
INC += ${TOOL_INC}
