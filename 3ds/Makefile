# GDWave 3DS build
TARGET      := GDWave3DS
BUILD       := build
SOURCES     := .
INCLUDES    := 
DATA        := 
GRAPHICS    := 
ROMFS       := ../romfs

# Libraries
LIBS        := -lcitro2d -lcitro3d -lm
ARCH        := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

# Default to .3dsx build
OUTPUT      := $(TARGET)

include $(DEVKITPRO)/3ds_rules
