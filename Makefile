
# Simple Makefile for desktop prototype (Linux)
CC = gcc
CFLAGS = -O2 `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lm
SRC = src/main.c
OBJ = $(SRC:.c=.o)
TARGET = GDWave

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
