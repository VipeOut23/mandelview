#ifndef __PIXBUF_H__
#define __PIXBUF_H__

#include <stdlib.h>
#include <stdint.h>


struct __PIXEL {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
typedef struct __PIXEL pixel_t;

struct __PIXBUF {
	size_t width;
	size_t height;
	struct __PIXEL *buf;
};
typedef struct __PIXBUF pixbuf_t;

enum __PNGERROR {
      ok,
      io,
      libpng,
      pnggen
};
typedef enum __PNGERROR pngerror_t;

void pixbuf_new(pixbuf_t **buf, const size_t width, const size_t height);

void pixbuf_destroy(pixbuf_t *buf);

pixel_t *pixbuf_pixel(pixbuf_t *buf, const off_t x, const off_t y);

int pixbuf_copy(pixbuf_t *dest, pixbuf_t *src);

int pixbuf_save_png(pixbuf_t *buf, const char *file);

int pixbuf_pixflood(pixbuf_t *buf, pixbuf_t *diff, int fd, int sx, int sy);

#endif
