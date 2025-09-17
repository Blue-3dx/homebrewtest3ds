# 3ds/Makefile — manual build for GDWave3DS (produces GDWave3DS.3dsx)
DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM

CC      := $(DEVKITARM)/bin/arm-none-eabi-gcc
AR      := $(DEVKITARM)/bin/arm-none-eabi-ar
OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy

CFLAGS   := -O2 -Wall -fdata-sections -ffunction-sections
CFLAGS  += -march=armv6k -mtune=mpcore -mfloat-abi=hard
INCLUDES := -I$(DEVKITPRO)/libctru/include -I$(DEVKITPRO)/citro2d/include

LDFLAGS  := -specs=3dsx.specs -Wl,-Map,$(notdir $@).map
LDFLAGS  += -L$(DEVKITPRO)/libctru/lib -lctru
LDFLAGS  += -L$(DEVKITPRO)/citro2d/lib -lcitro2d
LDFLAGS  += -lm

SRC := main.c
OBJ := $(SRC:.c=.o)

TARGET := GDWave3DS.3dsx

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "[Link] $@"
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	@echo "[Compile] $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *.map

.PHONY: all clean
