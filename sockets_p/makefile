# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wno-trigraphs

# Libraries
LIBS = -lncursesw -lmenuw

SRC = $(wildcard *.c) # compile all .c files in the directory
HEADERS = $(wildcard *.h)
TARGET = frogger_resurrection

all: $(TARGET)

# Build the target
$(TARGET):$(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean up build files
clean:
	rm -f $(TARGET)
