#include "mark.h"
#include "line.h"
#include "math.h"

GLushort Mark::_idc[2] = {
	0, 1
};

Mark* markMk(GLfloat val[2][2], unsigned int status) {
	Mark* _ = (Mark*) malloc(sizeof (Mark));

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 2; i++) {
			_->_val[j][i] = val[j][i];
		}
	}

	GLfloat vtc[2][3] = {
		0.0
	};
	for (int b = 0; b < 2; b++) {
		vtc[b][X] = _->_val[b][X];
		vtc[b][Z] = _->_val[b][Y];
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

	_->_parent = lineMk((GLfloat*) vtc, Mark::_idc, 2, "main", "thick", frag, active);

	return _;
}

void markDel(Mark* mark) {
}
