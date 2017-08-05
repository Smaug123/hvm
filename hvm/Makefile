CC=gcc
CFLAGS=-I. -Werror -Wextra

hvm: hvm.o
	$(CC) -o hvm main.c hvm.c $(CFLAGS)

clean:
	rm -f *.o

test: hvm.o
	./test.sh