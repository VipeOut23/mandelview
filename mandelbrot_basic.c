#include "mandelbrot_basic.h"

#include <math.h>


void center_viewbox(struct viewbox *v, const struct complex center,
			   const double zoom)
{
	double x_dist;
	double y_dist;
	double zoom_scale;

	zoom_scale = .5/zoom;

	x_dist = fabs(v->tl.real - v->br.real) * zoom_scale;
	y_dist = fabs(v->tl.imag - v->br.imag) * zoom_scale;

	v->tl.real = center.real - x_dist;
	v->tl.imag = center.imag + y_dist;
	v->br.real = center.real + x_dist;
	v->br.imag = center.imag - y_dist;
}

static inline struct complex translate_pixel_pos(const int x, const int x_dist,
						 const int y, const int y_dist,
						 const struct viewbox *v)
{
	struct complex c;
	double	       x_rel;
	double	       y_rel;
	double         vx_dist;
	double         vy_dist;

	x_rel = (double)x / (double)x_dist;
	y_rel = (double)y / (double)y_dist;
	vx_dist = fabs(v->tl.real - v->br.real);
	vy_dist = fabs(v->tl.imag - v->br.imag);

	c.real = v->tl.real + (x_rel*vx_dist);
	c.imag = v->tl.imag - (y_rel*vy_dist);	// x coordinates are top to bottom in pixbuf

	return c;
}

static inline int calculate_iterations(const struct complex *c, const int max_it)
{
	struct complex a;
	double	       a_rsq;
	double	       a_isq;

	a = *c;

	for(int i = 0; i < max_it; ++i) {
		/* square a's values */
		a_rsq = a.real*a.real;
		a_isq = a.imag*a.imag;

		if(a_rsq+a_isq > 4.0)		// abs(old_a) > 4
			return i;

		/* square a */
		a.imag = a.real*a.imag*2;
		a.real = a_rsq - a_isq;

		/* add c */
		a.real += c->real;
		a.imag += c->imag;
	}

	return max_it;
}

void calculate(pixbuf_t *pb, const struct viewbox *vb, const int iterations)
{
	struct complex c;
	pixel_t        *p;
	int            it;
		
#pragma omp parallel for schedule(dynamic, 10) private(c, p, it)
	for(off_t y = 0; y < pb->height; ++y) {
		for(off_t x = 0; x < pb->width; ++x) {
			p = pixbuf_pixel(pb, x, y);
			c = translate_pixel_pos(x, pb->width, y, pb->height, vb);
			it = calculate_iterations(&c, iterations);

			p->r = 255 * ((float)it/(float)iterations);
			p->g = 100 * ((float)it/(float)iterations);
			p->b = 55  * ((float)it/(float)iterations);
		}
	}
}
