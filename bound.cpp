#include "bound.h"
#include "util.h"

Bound* boundMk() {
	Bound* _ = (Bound*) malloc(sizeof (Bound));

	GLfloat vtc[2 * 2 * 2 * 3];
	for (int i = 0; i < 2 * 2 * 2 * 3; i++) {
		vtc[i] = 0.0;
	}
	std::vector<Obj*> scn = {
		objMk("wheel", "obj", "dir", true)
	};
	util::mesh::bound(vtc, scn);

	for (int i = 0; i < 2 * 2 * 2 * 3; i += 3) {
		std::cout << vtc[i] << std::endl;
		std::cout << vtc[i + 1] << std::endl;
		std::cout << vtc[i + 2] << std::endl;
		std::cout << std::endl;
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

	_->_parent = objMk(vtc, 2, idc, 3 * 2 * 3 * 2, "obj", "dir", true);

	return _;
}
