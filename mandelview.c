#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "mandelbrot_basic.h"
#include "pixbuf.h"

int main()
{
	int iterations = 250;
	int depth      = 20;
	double zoom    = 1.5;

	pixbuf_t *pb;
	struct viewbox vb;
	struct complex center;
	int ret;
	char buf[255];
	clock_t then, time_used, total = 0;

	vb.tl.real = -2.0;
	vb.tl.imag = 2.0;
	vb.br.real = 2.0;
	vb.br.imag = -2.0;

	center.real = 0.332711;
	center.imag = -0.53575;
	
	pixbuf_new(&pb, 1000, 1000);

 calculate:
	then = clock();
	calculate(pb, &vb, iterations);
	time_used = clock() - then;
	total += time_used;

	printf("Time: %lfs\n", ((double)time_used / CLOCKS_PER_SEC));

	sprintf(buf, "rendered/%d.png", depth);
	if( (ret = pixbuf_save_png(pb, buf)) ) {
		switch((pngerror_t)ret) {
		case libpng:
			fputs("libpng error...\n", stderr);
			break;
		case pnggen:
			fputs("Error, while generating png...\n", stderr);
			break;
		case io:
			fprintf(stderr, "Could not write to %s: %s\n",
				buf, strerror(errno));
			break;
		default:;
		}
		return -1;
	}

	if(--depth) {
		center_viewbox(&vb, center, zoom);
		goto calculate;
	}

	printf("Total: %lfs\n", ((double)total / CLOCKS_PER_SEC));

	pixbuf_destroy(pb);

	return 0;
}
