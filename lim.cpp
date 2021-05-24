#include <GL/glew.h>

#include "lim.h"
#include "util.h"
#include "layout.h"
#include "line.h"

Lim* limMk(unsigned int axis, GLfloat val) {
	Lim* _ = (Lim*) malloc(sizeof (Lim));

	_->_axis = axis;
	_->_val = val;

	GLfloat vtc[2 * 3];
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 3; i++) {
			if (!i) {
				vtc[(b * 3) + i] = (b ? 1 : -1) * 100.0;
			} else {
				vtc[(b * 3) + i] = 0.0;
			}
		}
	}

	glm::vec3 loc = glm::vec3(0.0);
	glm::vec3 rot = glm::vec3(0.0);

	switch (axis) {
		case X:
			loc[X] = val;
			rot[Y] = M_PI / 2;

			break;

		case Z:
			loc[Z] = val;
			rot[Y] = 0.0;

			break;
	}

	_->_parent = lineMk(vtc, "geom", "thick", "solid", loc, rot);

	return _;
}
