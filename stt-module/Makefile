#ARCH='arm'
#HOST?=$(ARCH)
#CROSS-COMPILE=$(ARCH)-elf-
CC=$(CROSS-COMPILE)gcc
PWD=$(shell pwd)

CFLAGS=-O2 -fPIC --shared
LDFLAGS=
LIBS=-ldl -lwiringPi -lpthread

TARGET=stt_module.so
OBJ=src/stt_module.c src/parson.c

%o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

install: 
	sudo mkdir -p /etc/3team/dl/
	sudo cp dl.rc /etc/3team/
	sudo cp $(TARGET) /etc/3team/dl/

.PHONY: clean

clean:
	rm -f src/*.o $(TARGET)
