CC=gcc
TARGET=main

all:	malloc.o testing.o
	$(CC) malloc.c testing.c -lpthread -o $(TARGET)

clean:
	rm *.o $(TARGET)
