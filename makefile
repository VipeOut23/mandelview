CC = gcc
CFLAGS = -Wall -lpng -fopenmp
OBJ = mandelbrot_basic.o pixbuf.o mandelview.o

mandelview: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o mandelview
debug: clean debug_flag mandelview

debug_flag: 
	$(eval CFLAGS += -g)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
tags:
	etags -a `(ls *.[ch])`
clean:
	rm -f *.o
