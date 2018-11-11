#ifndef __MANDELBROT_BASIC_H
#define __MANDELBROT_BASIC_H

#include "pixbuf.h"

struct complex {
	double real;
	double imag;
};

struct viewbox {
	struct complex tl; // top left corner
	struct complex br; // bottom right corner
};

void center_viewbox(struct viewbox *v, const struct complex center, const double zoom);

void calculate(pixbuf_t *pb, const struct viewbox *vb, const int iterations);

#endif
