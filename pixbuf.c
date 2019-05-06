#include "pixbuf.h"

#include <stdio.h>
#include <unistd.h>
#include <png.h>
#include <setjmp.h>
#include <string.h>

int pixel_equal(pixel_t *a, pixel_t *b)
{
		return a && b &&
				a->r == b->r &&
				a->g == b->g &&
				a->b == b->b;
}

void pixbuf_new(pixbuf_t **buf, const size_t width, const size_t height)
{
	*buf = malloc(sizeof(struct __PIXBUF));

	(*buf)->width = width;
	(*buf)->height = height;
	(*buf)->buf = calloc(width*height,sizeof(struct __PIXEL));
}

void pixbuf_destroy(pixbuf_t *buf)
{
	free(buf->buf);
	free(buf);
}

pixel_t *pixbuf_pixel(pixbuf_t *buf, const off_t x, const off_t y)
{
	return &buf->buf[y*buf->width + x];
}

int pixbuf_copy(pixbuf_t *dest, pixbuf_t *src)
{
		if(!src || !dest ||
		   src->width != dest->width ||
		   src->height != dest->height)
				return -1;

		memcpy(dest->buf, src->buf, src->height*src->width*sizeof(struct __PIXEL));
		return 0;
}

int pixbuf_pixflood(pixbuf_t *buf, pixbuf_t *diff, int fd, int sx, int sy)
{
		int yoff;
		int written;
		char str[128];
		pixel_t *p;

		for(int y = 0; y < buf->height; ++y) {
				yoff = sy+y;
				for(int x = 0; x < buf->width; ++x) {
						p = pixbuf_pixel(buf, x, y);

						/* do not redraw equal pixels */
						if( diff && pixel_equal(p, pixbuf_pixel(diff, x, y)) )
								goto next;

						written = sprintf(str, "PX %d %d %02x%02x%02x\n",
										  x+sx, yoff, p->r, p->g, p->b);

						if( write(fd, str, written) == -1 ) {
								perror("write()");
								return 1;
						}
				next:;

}
		}

		return 0;
}

int pixbuf_save_png(pixbuf_t *buf, const char *file)
{
	FILE *fp;
	png_structp png_p;
	png_infop   info_p;
	png_bytepp  rows;
	pngerror_t  ret;

	fp = fopen(file, "wb");
	if(!fp) {
		ret = io;
		goto fopen_fail;
	}
	

	png_p = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_p) {
		ret = libpng;
		goto png_struct_fail;

	}

	info_p = png_create_info_struct(png_p);
	if(!info_p) {
		ret = libpng;
		goto info_struct_fail;
	}

	if(setjmp(png_jmpbuf(png_p))) {
		ret = pnggen;
		goto png_fail;
	}
	
	png_set_IHDR(png_p, info_p,
		     buf->width, buf->height,
		     8, PNG_COLOR_TYPE_RGB,
		     PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_DEFAULT,
		     PNG_FILTER_TYPE_DEFAULT);

	rows = png_malloc(png_p, buf->height * sizeof(png_bytep));
	for(off_t y = 0; y < buf->height; ++y) {
		rows[y] = png_malloc(png_p, buf->width * 3);
		png_byte *row = rows[y];
		for(off_t x = 0; x < buf->width; ++x) {
			pixel_t *p = pixbuf_pixel(buf, x, y);
			*row++ = p->r;
			*row++ = p->g;
			*row++ = p->b;
		}
	}

	png_init_io(png_p, fp);
	png_set_rows(png_p, info_p, rows);
	png_write_png(png_p, info_p, PNG_TRANSFORM_IDENTITY, NULL);

	for(off_t y = 0; y < buf->height; ++y)
		png_free(png_p, rows[y]);
	png_free(png_p, rows);

	ret = ok;

 png_struct_fail:
 info_struct_fail:
	png_destroy_write_struct(&png_p, &info_p);
 png_fail:
	fclose(fp);
 fopen_fail:
	return ret;
}
