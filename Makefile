CC=gcc
CFLAGS=-I. -Werror -Wextra
HVM_SUBFOLDER=hvm

hvm: $(HVM_SUBFOLDER)/hvm.o
	$(CC) -o hvm_execute $(HVM_SUBFOLDER)/main.c $(HVM_SUBFOLDER)/hvm.c $(CFLAGS)

clean:
	rm -f $(HVM_SUBFOLDER)/*.o

test: $(HVM_SUBFOLDER)/hvm.o
	./test.sh