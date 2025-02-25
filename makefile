# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wno-trigraphs 

# Libraries
LIBS = -lncursesw -lmenuw -D_GNU_SOURCE -pthread

SRC = $(wildcard *.c) # compile all .c files in the directory
HEADERS = $(wildcard *.h)
TARGET = frogger_resurrection

# buffer = buffer.h struct.h utils.h 
# crocodile = crocodile.h struct.h utils.h design.h buffer.h game.h
# design = design.h struct.h frog.h crocodile.h game.h
# frog = frog.h design.h struct.h utils.h buffer.h game.h
# game = game.h crocodile.h frog.h design.h struct.h buffer.h utils.h
# main = main.h design.h frog.h game.h menu.h struct.h utils.h crocodile.h
# menu = menu.h
# struct = struct.h
# utils = utils.h design.h

# $(TARGET): buffer.o crocodile.o design.o frog.o game.o main.o menu.o utils.o
# 	${CC} $^ -o $@ ${CFLAGS} ${LIBS}

# buffer.o: buffer.c ${buffer}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# crocodile.o: crocodile.c ${crocodile}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# design.o: design.c ${design}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# frog.o: frog.c ${frog}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# game.o: game.c ${game}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# main.o: main.c ${main}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# menu.o: menu.c ${menu}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

# utils.o: utils.c ${utils}
# 	${CC} -c $*.c -o $@ ${CFLAGS} ${LIBS}

all: $(TARGET)

# Build the target
$(TARGET):$(SRC) $(HEADERS)
	$(CC) -o $(TARGET) $(SRC) $(LIBS) $(CFLAGS)

# Clean up build files
clean:
	rm -f frogger_resurrection
