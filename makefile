CC = gcc
CFLAGS = -Wall -lpng -g -o mandelview
SRC = mandelbrot.c pixbuf.c

mandelview: $(SRC)
	$(CC) $(CFLAGS) $(SRC)
tags:
	etags -a `(ls *.[ch])`
clean:
	rm -f *.o
