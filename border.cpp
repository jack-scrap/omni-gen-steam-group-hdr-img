#include "border.h"
#include "util.h"
#include "layout.h"
#include "state.h"
#include "util.h"

Border* borderMk(glm::vec2 sz, glm::vec3 loc, glm::vec3 rot) {
	Border* _ = (Border*) malloc(sizeof (Border));

	_->_sz = sz;

	GLfloat vtc[2 * 2 * 3 * 2];

	std::vector<GLfloat> outer = util::mesh::quad::pos1({
		sz[X] + (layout::stroke * 2), sz[Y] + (layout::stroke * 2)
	});
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = outer[i];
			vtc[i + 1] = outer[i + 1];
			vtc[i + 2] = outer[i + 2];

			i += 3;
		}
	}

	std::vector<GLfloat> inner = util::mesh::quad::pos1({
		sz[X], sz[Y]
	});
	i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[(2 * 2 * 3) + i] = inner[i] + layout::stroke;
			vtc[(2 * 2 * 3) + 1 + i] = inner[i + 1];
			vtc[(2 * 2 * 3) + 2 + i] = inner[i + 2] + layout::stroke;

			i += 3;
		}
	}

	GLushort
		strip[3 * 2 * 2 * 2],

		quad[2 * 2 * 2 * 2];

	// quad
	int cnt = 0;
	for (int q = 0; q < 2; q++) {
		i = 0;

		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				quad[cnt] = (q * 2) + (y * 2) + i;

				i++;
				cnt++;
			}
		}
	}

	for (int start = 0; start < 2; start++) {
		for (int i = 0; i < 2 * 2; i++) {
			quad[cnt] = start + (i * 2);

			cnt++;
		}
	}

	// strip
	bool far = false;

	for (int q = 0; q < 2 * 2; q++) {
		for (int t = 0; t < 2; t++) {
			for (int i = 0; i < 3; i++) {
				int
					start = (far * 2) + (2 * t),
					step = (i * (t ? -1 : 1)),
					last = (3 * (t && i == 2)),

					idx  = start + step + last;

				strip[((q * 2 * 3) + (t * 3)) + i] = quad[(q * (2 * 2)) + idx];
			}
		}
	}

	_->_parent = objMk(vtc, 2 * 2 * 2 * 2 * 3, strip, 3 * 2 * 2 * 2, "obj", "solid", true, loc, rot);

	return _;
}

Border* borderMk(glm::vec2 sz, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	Border* _ = (Border*) malloc(sizeof (Border));

	_->_sz = sz;

	GLfloat vtc[2 * 2 * 3 * 2];

	std::vector<GLfloat> outer = util::mesh::quad::pos1({
		sz[X] + (layout::stroke * 2), sz[Y] + (layout::stroke * 2)
	});
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = outer[i];
			vtc[i + 1] = outer[i + 1];
			vtc[i + 2] = outer[i + 2];

			i += 3;
		}
	}

	std::vector<GLfloat> inner = util::mesh::quad::pos1({
		sz[X], sz[Y]
	});
	i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[(2 * 2 * 3) + i] = inner[i] + layout::stroke;
			vtc[(2 * 2 * 3) + 1 + i] = inner[i + 1];
			vtc[(2 * 2 * 3) + 2 + i] = inner[i + 2] + layout::stroke;

			i += 3;
		}
	}

	GLushort
		strip[3 * 2 * 2 * 2],

		quad[2 * 2 * 2 * 2];

		// quad
		int cnt = 0;
		for (int q = 0; q < 2; q++) {
			i = 0;

			for (int y = 0; y < 2; y++) {
				for (int x = 0; x < 2; x++) {
					quad[cnt] = (q * 2) + (y * 2) + i;

					i++;
					cnt++;
				}
			}
		}

		for (int start = 0; start < 2; start++) {
			for (int i = 0; i < 2 * 2; i++) {
				quad[cnt] = start + (i * 2);

				cnt++;
			}
		}

		// strip
		bool far = false;

		for (int q = 0; q < 2 * 2; q++) {
			for (int t = 0; t < 2; t++) {
				for (int i = 0; i < 3; i++) {
					int
						start = (far * 2) + (2 * t),
						step = (i * (t ? -1 : 1)),
						last = (3 * (t && i == 2)),

						idx  = start + step + last;

					strip[((q * 2 * 3) + (t * 3)) + i] = quad[(q * (2 * 2)) + idx];
				}
			}
		}

		_->_parent = objMk(vtc, 2 * 2 * 2 * 2 * 3, strip, 3 * 2 * 2 * 2, "obj", "solid", true, child, noChild, loc, rot);

		return _;
}
