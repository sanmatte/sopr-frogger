# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Libraries
LIBS = -lncursesw

SRC = design.c game.c
TARGET = output

all: $(TARGET)

# Build the target
$(TARGET):$(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean up build files
clean:
	rm -f $(TARGET)
