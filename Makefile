CC=gcc
CFLAGS=-Wall -g

all:
	$(CC) $(CFLAGS) lastprogram.c -o lastprogram

clean:
	rm -rf lastprogram
