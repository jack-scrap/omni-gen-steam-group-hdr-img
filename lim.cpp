#include <GL/glew.h>

#include "lim.h"
#include "util.h"
#include "layout.h"
#include "line.h"

GLushort Lim::_idc[2] = {
	0, 1
};

Lim* limMk(unsigned int axis, GLfloat val) {
	Lim* _ = (Lim*) malloc(sizeof (Lim));

	_->_axis = axis;
	_->_val = val;

	GLfloat vtc[2][3];
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 3; i++) {
			if (i == axis) {
				vtc[b][i] = (b ? 1 : -1) * 100.0;
			} else {
				vtc[b][i] = 0.0;
			}
		}
	}

	_->_parent = lineMk((GLfloat*) vtc, Lim::_idc, 2, "main", "thick", "solid", true);

	return _;
}
