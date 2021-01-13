#include <iostream>
#include <sstream>

#include "util.h"
#include "math.h"

template <typename T>
T util::fs::rd(std::string name) {
}

template <>
std::string util::fs::rd<std::string>(std::string name) {
	std::ifstream in;
	in.open(name);

	std::string cont;
	for (std::string l; std::getline(in, l);) {
		cont += l + "\n";
	}

	in.close();

	return cont;
}

template <>
std::vector<std::string> util::fs::rd<std::vector<std::string>>(std::string name) {
	std::ifstream in;
	in.open("./" + name);

	std::vector<std::string> cont;
	for (std::string l; std::getline(in, l);) {
		cont.push_back(l);
	}

	in.close();

	return cont;
}

std::vector<std::string> util::str::split(std::string buff, char delim) {
	std::vector<std::string> tok;

	std::stringstream s(buff);
	std::string seg;
	while (std::getline(s, seg, delim)) {
		tok.push_back(seg);
	}

	return tok;
}

std::string util::str::join(std::vector<std::string> buff) {
	std::string _;
	for (const std::string& l : buff) {
		_ += l + '\n';
	}

	return _;
}

std::vector<GLfloat> util::mesh::plane(glm::vec2 sz) {
	std::vector<GLfloat> _;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_.push_back(x * sz[X]);
			_.push_back(y * sz[Y]);
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::rd::vtc(std::string name) {
	std::vector<GLfloat> obj;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "v") {
			for (int i = 1; i < 1 + 3; i++) {
				obj.push_back(std::stof(tok[i]));
			}
		}
	}

	return obj;
}

std::vector<GLushort> util::mesh::strip(std::vector<GLushort> idc) {
	std::vector<GLushort> _;

	bool far = false;

	for (int t = 0; t < 2; t++) {
		for (int i = 0; i < 3; i++) {
			int
				start = (far * 2) + (2 * t),
				step = (i * (t ? -1 : 1)),
				last = (3 * (t && i == 2));

			_.push_back(start + step + last);
		}
	}

	return _;
}

void util::mesh::bound(GLfloat bound[2 * 2 * 2 * 3], GLfloat* vtc, unsigned int noVtc) {
	// range
	GLfloat rng[3][2];
	for (int a = 0; a < 3; a++) {
		for (int i = 0; i < 2; i++) {
			rng[a][i] = 0.0;
		}
	}

	for (int v = 0; v < noVtc; v += 3) {
		for (int a = 0; a < 3; a++) {
			if (vtc[v + a] < rng[a][MIN]) {
				rng[a][MIN] = vtc[v + a];
			}

			if (vtc[v + a] > rng[a][MAX]) {
				rng[a][MAX] = vtc[v + a];
			}
		}
	}

	// generate
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				bound[i] = rng[X][x];
				bound[i + 1] = rng[Y][y];
				bound[i + 2] = rng[Z][z];

				i += 3;
			}
		}
	}
}

std::vector<GLfloat> util::mesh::quad::pos(glm::vec2 sz) {
	std::vector<GLfloat> _;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_.push_back((x ? 1 : -1) * sz[X]);
			_.push_back(((y ? 1 : -1)) * sz[Y]);
			_.push_back(0.0);
		}
	}

	return _;
}

std::vector<GLushort> util::mesh::quad::idc() {
	std::vector<GLushort> _;

	for (int t = 0; t < 2; t++) {
		for (int i = 0; i < 3; i++) {
			GLushort offset = (t * 3);

			_.push_back(offset + (i * (t ? -1 : 1)));
		}
	}

	return _;
}

std::vector<GLushort> util::mesh::rd::idc(std::string name) {
	std::vector<GLushort> obj;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "f") {
			for (int i = 1; i < 1 + 3; i++) {
				obj.push_back(std::stoi(tok[i]) - 1);
			}
		}
	}

	return obj;
}

bool util::phys::aabb(Obj* p, Obj* q) {
	bool _ = false;

	for (int a = 0; a < 2 * 2 * 2 * 3; a += 3) {
		glm::vec4 vtx = p->_model * glm::vec4(glm::vec3(p->_bound[a], p->_bound[a + 1], p->_bound[a + 2]), 1.0);

		for (int b = 0; b < 2 * 2 * 2 * 3; b += 3) {
			glm::vec4
				min = q->_model * glm::vec4(glm::vec3(q->_bound[b], q->_bound[b + 1], q->_bound[b + 2]), 1.0),
				max = q->_model * glm::vec4(glm::vec3(q->_bound[b], q->_bound[b + 1], q->_bound[b + 2]), 1.0);

			if (
				vtx[X] >= min[X] &&
				vtx[X] <= max[X] &&

				vtx[Y] >= min[Y] &&
				vtx[Y] <= max[Y] &&

				vtx[Z] >= min[Z] &&
				vtx[Z] <= max[Z]
			) {
				_ = true;
			}
		}
	}

	return _;
}
