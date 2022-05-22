#include "border.h"
#include "util.h"
#include "layout.h"
#include "state.h"
#include "util.h"

Border* borderMk(glm::vec2 bound, glm::vec3 loc, glm::vec3 rot) {
	Border* _ = (Border*) malloc(sizeof (Border));

	_->_bound = bound;

	GLfloat vtc[2 * 2 * 3 * 2];

	std::vector<GLfloat> outer = util::mesh::rect::pos({
		bound[X] + (layout::stroke * 2),
		bound[Y] + (layout::stroke * 2)
	}, Y, false);
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			for (int a = 0; a < 3; a++) {
				vtc[i] = outer[i];

				i++;
			}
		}
	}

	std::vector<GLfloat> inner = util::mesh::rect::pos({
		bound[X],
		bound[Y]
	}, Y, false);
	i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[(2 * 2 * 3) + i] = inner[i] + layout::stroke;
			vtc[(2 * 2 * 3) + 1 + i] = inner[i + 1];
			vtc[(2 * 2 * 3) + 2 + i] = inner[i + 2] + layout::stroke;

			i += 3;
		}
	}

	GLushort strip[3 * 2 * 2 * 2];

	GLushort rect[2 * 2 * 2 * 2];

	// rect
	int c = 0;
	for (int q = 0; q < 2; q++) {
		i = 0;

		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				rect[c] = (q * 2) + (y * 2) + i;

				i++;
				c++;
			}
		}
	}

	for (int start = 0; start < 2; start++) {
		for (int i = 0; i < 2 * 2; i++) {
			rect[c] = start + (i * 2);

			c++;
		}
	}

	// strip
	bool far = false;

	for (int q = 0; q < 2 * 2; q++) {
		for (int t = 0; t < 2; t++) {
			for (int i = 0; i < 3; i++) {
				int start = (far * 2) + (2 * t);
				int step = (i * (t ? -1 : 1));
				int last = (3 * (t && i == 2));

				int idx  = start + step + last;

				strip[((q * 2 * 3) + (t * 3)) + i] = rect[(q * (2 * 2)) + idx];
			}
		}
	}

	_->_parent = objMk(vtc, strip, 3 * 2 * 2 * 2, "obj", "solid", true, loc, rot);

	return _;
}

Border* borderMk(glm::vec2 bound, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	Border* _ = (Border*) malloc(sizeof (Border));

	_->_bound = bound;

	GLfloat vtc[2 * 2 * 3 * 2];

	std::vector<GLfloat> outer = util::mesh::rect::pos({
		layout::item(bound[X] + (layout::stroke * 2)),
		layout::item(bound[Y] + (layout::stroke * 2))
	}, Y, false);
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = outer[i];
			vtc[i + 1] = outer[i + 1];
			vtc[i + 2] = outer[i + 2];

			i += 3;
		}
	}

	std::vector<GLfloat> inner = util::mesh::rect::pos({
		layout::item(bound[X]),
		layout::item(bound[Y])
	}, Y, false);
	i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[(2 * 2 * 3) + i] = inner[i] + layout::stroke;
			vtc[(2 * 2 * 3) + 1 + i] = inner[i + 1];
			vtc[(2 * 2 * 3) + 2 + i] = inner[i + 2] + layout::stroke;

			i += 3;
		}
	}

	GLushort strip[3 * 2 * 2 * 2];

	GLushort rect[2 * 2 * 2 * 2];

	// rect
	int c = 0;
	for (int q = 0; q < 2; q++) {
		i = 0;

		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				rect[c] = (q * 2) + (y * 2) + i;

				i++;
				c++;
			}
		}
	}

	for (int start = 0; start < 2; start++) {
		for (int i = 0; i < 2 * 2; i++) {
			rect[c] = start + (i * 2);

			c++;
		}
	}

	// strip
	bool far = false;

	for (int q = 0; q < 2 * 2; q++) {
		for (int t = 0; t < 2; t++) {
			for (int i = 0; i < 3; i++) {
				int start = (far * 2) + (2 * t);
				int step = (i * (t ? -1 : 1));
				int last = (3 * (t && i == 2));

				int idx  = start + step + last;

				strip[((q * 2 * 3) + (t * 3)) + i] = rect[(q * (2 * 2)) + idx];
			}
		}
	}

	_->_parent = objMk(vtc, strip, 3 * 2 * 2 * 2, "obj", "solid", true, child, noChild, loc, rot);

	return _;
}

void borderDel(Border* border) {
	objDel(border->_parent);

	free(border);
}
