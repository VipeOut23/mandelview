#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pixbuf.h"

struct complex {
	double real;
	double imag;
};

struct viewbox {
	struct complex tl; // top left corner
	struct complex br; // bottom right corner
};

static void center_viewbox(struct viewbox *v, const struct complex center,
			   const int zoom)
{
	double x_dist;
	double y_dist;
	double zoom_scale;

	zoom_scale = .5f/zoom;

	x_dist = fabs(v->tl.real - v->br.real) * zoom_scale;
	y_dist = fabs(v->tl.imag - v->br.imag) * zoom_scale;

	v->tl.real = center.real - x_dist;
	v->tl.imag = center.imag + y_dist;
	v->br.real = center.real + x_dist;
	v->br.imag = center.imag - y_dist;
}

static struct complex translate_pixel_pos(const int x, const int x_width,
					  const int y, const int y_width,
					  const struct viewbox *v)
{
	struct complex c;
	double	       x_rel;
	double	       y_rel;
	double         vx_dist;
	double         vy_dist;

	x_rel = (double)x / x_width;
	y_rel = (double)y / y_width;
	vx_dist = fabs(v->tl.real - v->br.real);
	vy_dist = fabs(v->tl.imag - v->br.imag);

	c.real = v->tl.real + x_rel*vx_dist;
	c.imag = v->tl.imag - y_rel*vy_dist;	// x coordinates are top to bottom in pixbuf

	return c;
}

static int calculate_iterations_64(const struct complex *c, const int max_it)
{
	struct complex a;
	float	       a_rsq;
	float	       a_isq;

	a = *c;

	for(int i = 0; i < max_it; ++i) {
		/* square a's dimensions */
		a_rsq = a.real*a.real;
		a_isq = a.imag*a.imag;

		if(a_rsq+a_isq > 4.0)		// abs(old_a) > 4
			return i;

		/* square a */
		a.real = a_rsq - a_isq;
		a.imag = a.real*a.imag*2.0;

		/* add c */
		a.real += c->real;
		a.imag += c->imag;
	}

	return max_it;
}

int main()
{
	int iterations = 200;
	int depth      = 10;

	pixbuf_t *pb;
	pixel_t  *p;
	struct viewbox vb;
	struct complex c;
	struct complex center;
	int ret;
	int it;
	char buf[255];

	vb.tl.real = -2.0;
	vb.tl.imag = 2.0;
	vb.br.real = 2.0;
	vb.br.imag = -2.0;

	center.real = -0.50;
	center.imag = 1.0;
	
	pixbuf_new(&pb, 400, 400);

 calculate:
	for(off_t y = 0; y < pb->height; ++y) {
		for(off_t x = 0; x < pb->width; ++x) {
			p = pixbuf_pixel(pb, x, y);
			c = translate_pixel_pos(x, pb->width, y, pb->height, &vb);
			it = calculate_iterations_64(&c, iterations);

			p->r = 255.0 * ((float)it/(float)iterations);
			p->g = 0;
			p->b = 0;
		}
	}

	sprintf(buf, "rendered/%d.png", depth);
	if( (ret = pixbuf_save_png(pb, buf)) ) {
		switch((pngerror_t)ret) {
		case libpng:
			fputs("libpng error...", stderr);
			break;
		case pnggen:
			fputs("Error, while generating png...", stderr);
			break;
		case io:
			fputs("Could not write to file.", stderr);
			break;
		default:;
		}
	}

	if(--depth) {
		center_viewbox(&vb, center, 2);
		goto calculate;
	}

	pixbuf_destroy(pb);
}
