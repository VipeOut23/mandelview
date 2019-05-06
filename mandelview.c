#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "mandelbrot_basic.h"
#include "pixbuf.h"

int main(int argc, char **argv)
{
	int iterations = 1500;
	int depth      = 40;
	double zoom    = 1.6;

	pixbuf_t *pb, *pb_old;
	struct viewbox vb;
	struct complex center;
	clock_t then, time_used;
	clock_t total_render = 0, total_send = 0;

	struct sockaddr_in addr;
	int sockfd;

	short port = 1234;

	switch(argc) {
	case 2: break;
	case 3: port = atoi(argv[2]);
			break;
	case 4: iterations = atoi(argv[3]);
			break;
	default: return 1;
	}

	vb.tl.real = -2.0;
	vb.tl.imag = 2.0;
	vb.br.real = 2.0;
	vb.br.imag = -2.0;

	center.real = -0.749948;
	center.imag = 0.022877;

	pixbuf_new(&pb, 300, 300);
	pixbuf_new(&pb_old, pb->width, pb->height);

	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	signal(SIGPIPE, SIG_IGN);
	if( connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) ) {
			perror("connect()");
			return 1;
	}

	pixbuf_set(pb, (pixel_t){.r = 255, .g = 255, .b = 255});

calculate:
	pixbuf_copy(pb_old, pb);

	then = clock();
	calculate(pb, &vb, iterations);
	time_used = clock() - then;
	total_render += time_used;

	printf("Render time: %lfs\n", ((double)time_used / CLOCKS_PER_SEC));

	then = clock();
	if( pixbuf_pixflood(pb, pb_old, sockfd, 0, 0) )
			return -1;
	time_used = clock() - then;
	total_send += time_used;

	printf("Send time: %lfs\n", ((double)time_used / CLOCKS_PER_SEC));

	if(--depth) {
		center_viewbox(&vb, center, zoom);
		goto calculate;
	}

	printf("Total render: %lfs\n", ((double)total_render / CLOCKS_PER_SEC));
	printf("Total send:   %lfs\n", ((double)total_send / CLOCKS_PER_SEC));

	pixbuf_destroy(pb);
	pixbuf_destroy(pb_old);

	return 0;
}
