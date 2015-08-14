CC = gcc

LOADER_DIR = /usr/lib/         #make sure that libloader.so has moved into /usr/lib
LOADER_LIB = $(LOADER_DIR)    


all        : test testlib

test       : testloader
testloader : testloader.c
	$(CC)  $^  -o testloader  -L$(LOADER_LIB)   -lloader

testlib    : demolib.so
demolib.so : demolib.c
	$(CC) -shared  $^  -o demolib.so

clean:
	rm -rf testloader demolib.so
