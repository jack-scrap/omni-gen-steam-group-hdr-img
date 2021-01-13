#include "bound.h"

Bound* boundMk() {
	Bound* _ = (Bound*) malloc(sizeof (Bound));

	GLfloat vtc[2 * 2 * 2 * 3];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				vtc[i] = x ? 1 : -1;
				vtc[i + 1] = y ? 1 : -1;
				vtc[i + 2] = z ? 1 : -1;

				i += 3;
			}
		}
	}

	GLushort idc[3 * 2 * 3 * 2] = {
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
	};
	
	_->_parent = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "obj", "dir", true);

	return _;
}
