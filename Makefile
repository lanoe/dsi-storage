.PHONY: clean

CC=gcc
CFLAGS=-I ./libto/include/
LIBS=-lTO
BINARY=encrypt-dsi-storage decrypt-dsi-storage 

all: $(BINARY)

encrypt-dsi-storage: encrypt-dsi-storage.o
	$(CC) -o encrypt-dsi-storage encrypt-dsi-storage.c $(CFLAGS) $(LIBS)

decrypt-dsi-storage: decrypt-dsi-storage.o
	$(CC) -o decrypt-dsi-storage decrypt-dsi-storage.c $(CFLAGS) $(LIBS)

clean:
	rm -f $(BINARY) *.o
