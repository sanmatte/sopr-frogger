# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Libraries
LIBS = -lncursesw

SRC = $(wildcard *.c) # compile all .c files in the directory
TARGET = output

all: $(TARGET)

# Build the target
$(TARGET):$(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean up build files
clean:
	rm -f $(TARGET)
