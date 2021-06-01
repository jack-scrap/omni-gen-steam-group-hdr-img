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

	GLfloat vtc[2][3];
	for (int b = 0; b < 2; b++) {
		for (int a = 0; a < 3; a++) {
			vtc[b][a] = 0.0;
		}

		switch (axis) {
			case X:
				vtc[b][X] = val;
				vtc[b][Z] = (b ? 1 : -1) * 100.0;
				
				break;

			case Z:
				vtc[b][Z] = val;
				vtc[b][X] = (b ? 1 : -1) * 100.0;

				break;
		}
	}

	std::string frag;
	bool active;
	switch (status) {
		case Lim::PASS:
			frag = "solid";
			active = false;

			break;

		case Lim::HALT:
			frag = "solid";
			active = true;

			break;

		case Lim::ALERT:
			frag = "alertLine";
			active = true;

			break;
	}

	_->_parent = lineMk((GLfloat*) vtc, Lim::_idc, 2, "main", "thick", frag, active);

	return _;
}
