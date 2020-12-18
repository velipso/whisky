//
// Whisky - a collection of stateless RNGs
// by Sean Connelly (@velipso), https://sean.cm
// The Unlicense License
// Project Home: https://github.com/velipso/whisky
//
// Compile via:
//   clang -o whisky.out -O2 test.c
//
// Create reports using the included bash script in this repo:
//   ./report 1alt
//

#include "whisky.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t (*dim1_f)(uint32_t x0);
static struct {
	const char *sig;
	int dim;
	union {
		dim1_f f1;
	} u;
} streams[] = {
	{ .sig = "1", .dim = 1, .u.f1 = whisky1 },
	{ .sig = "1alt", .dim = 1, .u.f1 = whisky1alt },
	{ .sig = "1_fast", .dim = 1, .u.f1 = whisky1_fast },
	{ .sig = "1alt_fast", .dim = 1, .u.f1 = whisky1alt_fast },
	{ .dim = 0 }
};

static int usage(int ret){
	printf(
		"Streams random numbers out of stdout based on the given generator.\n\n"
		"Designed to be used with the ./report bash script.\n\n"
		"Usage:\n"
		"  whisky.out <generator> <axis> <dir>\n\n"
		"Where:\n\n"
		"  <generator>   The whisky generator to use, ex, 1alt\n"
		"  <axis>        The axis to march along, ex, 1, 2, etc\n"
		"  <dir>         The direction to march on the axis, '+' or '-'\n\n"
		"Example:\n"
		"  whisky.out 1alt 0 +\n\n"
		"Available generators:\n"
	);
	int i = 0;
	while (streams[i].dim){
		printf(" %10s", streams[i].sig);
		i++;
		if ((i % 6) == 0 && streams[i].dim)
			printf("\n");
	}
	if ((i % 6) != 0)
		printf("\n");
	return ret;
}

static int usageaxis(int axis, int dim){
	fprintf(stderr,
		"Invalid axis value of %d\n"
		"The selected generator has %d dimension%s,\n"
		"so axis must range from 1 to %d\n",
		axis, dim, dim == 1 ? "" : "s", dim);
	return 1;
}

#define BUF 1000
static int stream1(int axis, int dir, dim1_f f){
	if (axis != 1)
		return usageaxis(axis, 1);
	uint32_t v[BUF];
	uint32_t n = 0;
	while (1){
		for (int i = 0; i < BUF; i++, n += dir)
			v[i] = f(n);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

int main(int argc, char **argv){
	if (argc < 2)
		return usage(0);
	else if (argc == 3 || (argc >= 4 && strcmp(argv[3], "+") && strcmp(argv[3], "-")))
		return usage(1);
	else if (argc == 2){
		// test if generator exists
		int i = 0;
		while (streams[i].dim){
			if (strcmp(argv[1], streams[i].sig) == 0)
				return 0;
			i++;
		}
		return 1;
	}

	// run a generator along the axis
	int axis = atoi(argv[2]);
	int dir = strcmp(argv[3], "+") == 0 ? 1 : -1;
	int i = 0;
	while (streams[i].dim){
		if (strcmp(argv[1], streams[i].sig) == 0){
			switch (streams[i].dim){
				case 1: return stream1(axis, dir, streams[i].u.f1);
			}
		}
		i++;
	}

	fprintf(stderr, "Couldn't find generator %s\n", argv[1]);
	return usage(1);
}
