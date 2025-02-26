# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wno-trigraphs 

# Libraries
LIBS = -lncursesw -lmenuw -D_GNU_SOURCE -pthread

SRC = $(wildcard *.c) # compile all .c files in the directory
HEADERS = $(wildcard *.h)
TARGET = frogger_resurrection

all: $(TARGET)

# Build the target
$(TARGET):$(SRC) $(HEADERS)
	$(CC) -o $(TARGET) $(SRC) $(LIBS) $(CFLAGS)

# Clean up build files
clean:
	rm -f frogger_resurrection
