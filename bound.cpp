#include "bound.h"
#include "util.h"

Bound* boundMk(std::vector<Obj*> scn) {
	Bound* _ = (Bound*) malloc(sizeof (Bound));

	GLfloat vtc[2 * 2 * 2 * 3];
	util::mesh::bound(vtc, scn);

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

	_->_parent = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "obj", "solid", true);

	return _;
}
