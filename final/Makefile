CC=gcc

TESTS=testing
HEADERS=malloc.h

all:	helper.o malloc.o realloc.o reallocarray.o calloc.o memalign.o posix_memalign.o free.o testing.o
	$(CC) helper.c malloc.c realloc.c reallocarray.c calloc.c memalign.c posix_memalign.c free.c testing.c -lpthread -o $(TESTS)

clean:
	rm *.o ${TESTS}
