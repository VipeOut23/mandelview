CC = gcc
CFLAGS = -Wall -lpng -fopenmp
ALLCFLAGS = $(CFLAGS) $(shell echo | gcc -xc -E -v - 2>&1 | grep -E '^\s' | sed '1d;s/^\s/ -I/' | tr '\n' ' ') # Explictly include system libraries for cdb
OBJ = mandelbrot_basic.o pixbuf.o mandelview.o

mandelview: $(OBJ)
	$(CC) $(ALLCFLAGS) $(OBJ) -o mandelview
debug: clean debug_flag mandelview

debug_flag: 
	$(eval ALLCFLAGS += -g)
%.o: %.c
	$(CC) $(ALLCFLAGS) -c $< -o $@
tags:
	etags -a `(ls *.[ch])`
clean:
	rm -f *.o
