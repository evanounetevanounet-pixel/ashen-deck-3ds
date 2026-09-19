TARGET := 3ds_card_roguelike
BUILD := build
SOURCES := source

include $(DEVKITARM)/base_rules

ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS := -g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH) -D__3DS__
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS := -specs=3dsx.specs $(ARCH) -Wl,-Map,$(TARGET).map
LIBS := -lctru -lm

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
OFILES := $(CFILES:.c=.o)
OFILES := $(foreach f,$(OFILES),$(BUILD)/$(f))

.PHONY: all clean

all: $(TARGET).3dsx

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: $(SOURCES)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(TARGET).3dsx: $(TARGET).elf
	3dsxtool $< $@

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).3dsx $(TARGET).map
