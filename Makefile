TARGET := 3ds_card_roguelike

BUILD := build
SOURCES := source
INCLUDES :=

ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS := -g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH)
CFLAGS += $(INCLUDE) -D__3DS__

CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)

LIBS := -lctru -lm

include $(DEVKITARM)/3ds_rules
