CC = gcc
CFLAGS = -Wall -lpng -g -fopenmp
OBJ = mandelbrot_basic.o pixbuf.o mandelview.o

mandelview: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o mandelview
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
tags:
	etags -a `(ls *.[ch])`
clean:
	rm -f *.o
