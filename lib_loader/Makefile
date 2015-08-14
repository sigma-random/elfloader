CC = gcc
CFLAGS  =  -shared 


INCLUDE = ./include/
CFILES  = $(wildcard src/*.c)
OBJS    = $(CFILES:%.c=%.o)
SWPS    = $(CFILES:%.c=%.o)

TARGET = libloader.so

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC)  $(CFLAGS)  $^  -o $(TARGET) $(LDFLAGS)

.c.o:
	$(CC)  $(CFLAGS) -I$(INCLUDE)  -c $^ -o $@ 

install:
	sudo cp $(TARGET) /usr/lib/$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET) 
