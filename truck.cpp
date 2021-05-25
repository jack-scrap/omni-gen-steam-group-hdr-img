#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"
#include "omni.h"

GLfloat Truck::_vtx[3] = {
	0.0, 0.0, 0.0
};

GLushort Truck::_idx[1] = {
	0
};

Truck* truckMk(glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_no = 0;

	_->_ang = 0.0;

	unsigned int w = _->_no * 2;

	Obj* child[(w * 2) + 1 + 1];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			Obj* rim[] = {
				objMk("rim", "obj", "dir", true)
			};

			child[i] = objMk("wheel", "obj", "dir", false, rim, sizeof rim / sizeof *rim, glm::vec3(x * -2.6, -1, z ? 1 : -1), glm::vec3(0.0, z * M_PI, 0.0));

			i++;
		}
	}

	child[w * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(2 * layout::idx[Z]) - (layout::stroke) - (layout::stroke * 2), 0.0, 0.0), rot);

	child[(w * 2) + 1] = nullptr;

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

Truck* truckMk(char* init, unsigned int no, glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_no = no;

	_->_ang = 0.0;

	unsigned int w = _->_no * 2;

	Obj* child[(w * 2) + 1 + 2];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < w; x++) {
			Obj* rim[] = {
				objMk("rim", "obj", "dir", true)
			};

			child[i] = objMk("wheel", "obj", "dir", false, rim, sizeof rim / sizeof *rim, glm::vec3(x * -2.6, -1, z ? 1 : -1), glm::vec3(0.0, z * M_PI, 0.0));

			i++;
		}
	}

	child[w * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(no * layout::idx[Z]) - (layout::stroke) - (layout::stroke * 2), 0.0, 0.0), rot);

	Obj* bed = ptMk(Truck::_vtx, "geom", "bed", "dir", glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	bed->_prog.use();

	_->_uni[Truck::SZ_BED] = glGetUniformLocation(bed->_prog._id, "sz");

	glUniform1ui(_->_uni[Truck::SZ_BED], no);

	bed->_prog.unUse();

	Obj* outer = ptMk(Truck::_vtx, "geom", "outer", "dir", glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	outer->_prog.use();

	_->_uni[Truck::SZ_OUTER] = glGetUniformLocation(outer->_prog._id, "sz");

	glUniform1ui(_->_uni[Truck::SZ_OUTER], no);

	outer->_prog.unUse();

	child[(w * 2) + 1] = bed;
	child[(w * 2) + 1 + 1] = outer;

	_->_data = arrMk(init, no, "", glm::vec3(0.0, 0.0, -((layout::idx[X] / 2) + (layout::stroke * 2) + (layout::margin * 2 * 2))), glm::vec3(0.0, -M_PI / 2, 0.0));

	child[(w * 2) + 1 + 1] = _->_data->_parent;

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

void truckTurn(Truck* truck, float delta) {
	if (
		truck->_ang + delta > truck->_rngWheel[MIN] &&
		truck->_ang + delta < truck->_rngWheel[MAX]
	) {
		truck->_ang += delta;

		for (int z = 0; z < 2; z++) {
			objAnim(truck->_parent->_child[z * 2], glm::vec3(0.0), glm::vec3(0.0, delta, 0.0));
		}
	} else {
		omni::err("Cannot turn truck; rotation of wheels exceeds range");
	}

	glm::vec3 offset = truck->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		truck->_loc[i] = offset[i];
	}
}

void truckMv(Truck* truck, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	objAnim(truck->_parent, dest, glm::vec3(0.0));

	glm::vec3 offset = truck->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		truck->_loc[i] = offset[i];
	}
}
