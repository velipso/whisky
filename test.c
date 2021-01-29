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

typedef uint32_t (*dim1_f)(uint32_t i0);
typedef uint32_t (*dim2_f)(uint32_t i0, uint32_t i1);
typedef uint32_t (*dim3_f)(uint32_t i0, uint32_t i1, uint32_t i2);
typedef uint32_t (*dim4_f)(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3);
typedef uint32_t (*dim5_f)(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4);

static struct {
	const char *sig;
	int dim;
	union {
		dim1_f f1;
		dim2_f f2;
		dim3_f f3;
		dim4_f f4;
		dim5_f f5;
	} u;
} streams[] = {
	{ .sig = "1"     , .dim = 1, .u.f1 = whisky1         },
	{ .sig = "1alt"  , .dim = 1, .u.f1 = whisky1alt      },
	{ .sig = "2"     , .dim = 2, .u.f2 = whisky2         },
	{ .sig = "2alt"  , .dim = 2, .u.f2 = whisky2alt      },
	{ .sig = "3"     , .dim = 3, .u.f3 = whisky3         },
	{ .sig = "3alt"  , .dim = 3, .u.f3 = whisky3alt      },
	{ .sig = "4"     , .dim = 4, .u.f4 = whisky4         },
	{ .sig = "4alt"  , .dim = 4, .u.f4 = whisky4alt      },
	{ .sig = "5"     , .dim = 5, .u.f5 = whisky5         },
	{ .sig = "5alt"  , .dim = 5, .u.f5 = whisky5alt      },
	{ .sig = "5alt2" , .dim = 5, .u.f5 = whisky5alt2     },
	{ .sig = "sha256", .dim = 8 },
	{ .dim = 0 }
};

static int usage(int ret){
	printf(
		"Streams random numbers out of stdout based on the given generator.\n\n"
		"Designed to be used with the ./report bash script.\n\n"
		"Usage:\n"
		"  whisky.out <generator> <axis> <dir>\n\n"
		"Where:\n"
		"  <generator>   The whisky generator to use, ex, 1alt\n"
		"  <axis>        The axis to march along, ex, 1, 2, etc\n"
		"  <dir>         The direction to march on the axis, '+' or '-'\n\n"
		"Example:\n"
		"  whisky.out 1alt 1 +\n\n"
		"Available generators:\n"
	);
	int i = 0;
	while (streams[i].dim){
		printf("  %s ", streams[i].sig);
		i++;
		if ((i % 4) == 0 && streams[i].dim)
			printf("\n");
	}
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
	uint32_t v[BUF], n = 0;
	while (1){
		for (int i = 0; i < BUF; i++, n += dir)
			v[i] = f(n);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

static int stream2(int axis, int dir, dim2_f f){
	if (axis < 1 || axis > 2)
		return usageaxis(axis, 2);
	uint32_t v[BUF], n[2] = {0};
	axis--;
	while (1){
		for (int i = 0; i < BUF; i++, n[axis] += dir)
			v[i] = f(n[0], n[1]);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

static int stream3(int axis, int dir, dim3_f f){
	if (axis < 1 || axis > 3)
		return usageaxis(axis, 3);
	uint32_t v[BUF], n[3] = {0};
	axis--;
	while (1){
		for (int i = 0; i < BUF; i++, n[axis] += dir)
			v[i] = f(n[0], n[1], n[2]);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

static int stream4(int axis, int dir, dim4_f f){
	if (axis < 1 || axis > 4)
		return usageaxis(axis, 4);
	uint32_t v[BUF], n[4] = {0};
	axis--;
	while (1){
		for (int i = 0; i < BUF; i++, n[axis] += dir)
			v[i] = f(n[0], n[1], n[2], n[3]);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

static int stream5(int axis, int dir, dim5_f f){
	if (axis < 1 || axis > 5)
		return usageaxis(axis, 5);
	uint32_t v[BUF], n[5] = {0};
	axis--;
	while (1){
		for (int i = 0; i < BUF; i++, n[axis] += dir)
			v[i] = f(n[0], n[1], n[2], n[3], n[4]);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

static int stream8(int axis, int dir){
	if (axis < 1 || axis > 8)
		return usageaxis(axis, 8);
	uint32_t v[BUF], n[8] = {0};
	axis--;
	while (1){
		for (int i = 0; i < BUF; i += 8, n[axis] += dir)
			whisky_sha256(n, &v[i]);
		fwrite(v, sizeof(uint32_t), BUF, stdout);
	}
	return 0;
}

int main(int argc, char **argv){
	// while we're here, let's just verify that our sha256 hash is correct
	{
		// require('crypto')
		//   .createHash('sha256')
		//   .update('youre in the demon produce aisle')
		//   .digest('hex')
		// '1e27be8c998b6128d2ce64866510802a92eb83bdb1208263780cf8275d41437b'
		uint32_t d[8] = {
			0x796f7572, 0x6520696e, 0x20746865, 0x2064656d,
			0x6f6e2070, 0x726f6475, 0x63652061, 0x69736c65
		};
		whisky_sha256(d, d);
		if (d[0] != 0x1e27be8c || d[1] != 0x998b6128 || d[2] != 0xd2ce6486 || d[3] != 0x6510802a ||
			d[4] != 0x92eb83bd || d[5] != 0xb1208263 || d[6] != 0x780cf827 || d[7] != 0x5d41437b){
			fprintf(stderr, "whisky_sha256 is wrong!\n");
			return 1;
		}
	}

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
				case 2: return stream2(axis, dir, streams[i].u.f2);
				case 3: return stream3(axis, dir, streams[i].u.f3);
				case 4: return stream4(axis, dir, streams[i].u.f4);
				case 5: return stream5(axis, dir, streams[i].u.f5);
				case 8: return stream8(axis, dir);
			}
		}
		i++;
	}

	fprintf(stderr, "Invalid generator: %s\n", argv[1]);
	return usage(1);
}
