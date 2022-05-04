#include <GL/glew.h>

#include "lim.h"
#include "util.h"
#include "layout.h"
#include "line.h"

GLushort Lim::_idc[2] = {
	0, 1
};

Lim* limMk(unsigned int axis, GLfloat val, unsigned int status) {
	Lim* _ = (Lim*) malloc(sizeof (Lim));

	_->_axis = axis;
	_->_val = val;

	GLfloat vtc[2][3] = {
		0.0
	};
	for (int b = 0; b < 2; b++) {
		switch (axis) {
			case X:
				vtc[b][X] = _->_val;
				vtc[b][Z] = (b ? 1 : -1) * 100.0;
				
				break;

			case Z:
				vtc[b][Z] = _->_val;
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

	_->_parent = lineMk((GLfloat*) vtc, Lim::_idc, 2, "main", "thick", frag, active);

	return _;
}
