#include <GL/glew.h>

#include "lim.h"
#include "util.h"
#include "layout.h"
#include "line.h"

GLushort Lim::_idc[2] = {
	0, 1
};

Lim* limMk(unsigned int axis, GLfloat val, unsigned int status) {
	Lim* inst = (Lim*) malloc(sizeof (Lim));

	inst->axis = axis;
	inst->val = val;

	GLfloat vtc[2][3];
	for (int b = 0; b < 2; b++) {
		vtc[b][Y] = 0.0;

		switch (axis) {
			case X:
				vtc[b][X] = inst->val;
				vtc[b][Z] = (b ? 1 : -1) * 100.0;
				
				break;

			case Z:
				vtc[b][Z] = inst->val;
				vtc[b][X] = (b ? 1 : -1) * 100.0;

				break;
		}
	}

	std::string frag;
	bool active;
	switch (status) {
		case PASS:
			frag = "solid";
			active = false;

			break;

		case HALT:
			frag = "solid";
			active = true;

			break;

		case ALERT:
			frag = "alert";
			active = true;

			break;
	}

	inst->_parent = lineMk((GLfloat*) vtc, Lim::_idc, 2, "main", "thick", frag, active);

	return inst;
}

void limDel(Lim* inst) {
	objDel(inst->_parent);

	free(inst);
}
