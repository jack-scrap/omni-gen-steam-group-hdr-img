#include "mark.h"
#include "line.h"
#include "math.h"
#include "layout.h"

GLushort Mark::_idc[2] = {
	0, 1
};

Mark* markMk(GLfloat val[2][2], unsigned int status) {
	Mark* inst = (Mark*) malloc(sizeof (Mark));

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 2; i++) {
			inst->_val[j][i] = val[j][i];
		}
	}

	GLfloat vtc[2][3];
	for (int b = 0; b < 2; b++) {
		vtc[b][X] = inst->_val[b][X];
		vtc[b][Y] = 0.0;
		vtc[b][Z] = inst->_val[b][Y];
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

	inst->_parent = lineMk((GLfloat*) vtc, Mark::_idc, 2, "main", "thick", frag, active);

	return inst;
}

void markDel(Mark* inst) {
	objDel(inst->_parent);

	free(inst);
}
