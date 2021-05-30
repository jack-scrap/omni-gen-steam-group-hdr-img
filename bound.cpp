#include "bound.h"
#include "util.h"
#include "math.h"

static GLushort Bound::_idc[3 * 2 * 3 * 2] = {
	0, 1, 2,
	2, 1, 3,

	4, 5, 6,
	6, 5, 7,

	0, 4, 1,
	1, 4, 5,

	2, 6, 3,
	3, 6, 7,

	0, 4, 2,
	2, 4, 6,

	1, 5, 3,
	3, 5, 7
}

Bound* boundMk(Obj** obj, unsigned int sz) {
	Bound* _ = (Bound*) malloc(sizeof (Bound));

	GLfloat vtc[2 * 2 * 2 * 3];

	// range
	GLfloat rng[3][2];
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(obj, sz, rng, glm::mat4(1.0));

	// generate
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				vtc[i] = rng[X][x];
				vtc[i + 1] = rng[Y][y];
				vtc[i + 2] = rng[Z][z];

				i += 3;
			}
		}
	}

	_->_parent = objMk(vtc, 2 * 2 * 2 * 3, Bound::_idc, 3 * 2 * 3 * 2, "obj", "trans", true);

	return _;
}
