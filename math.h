#pragma once

enum axes {
	X,
	Y,
	Z,
	W
};

enum rng {
	MIN,
	MAX
};

enum col {
	R,
	G,
	B,
	A
};

typedef struct {
	unsigned int
		_x,
		_y;
} Coord;
