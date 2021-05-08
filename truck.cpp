#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"
#include "omni.h"

Truck* truckMk(glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_ptr = nullptr;
	_->_no = 0;

	_->_ang = 0.0;

	unsigned int w = 2;

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

	child[w * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(2 * layout::idx[Y]) - (layout::stroke) - (layout::stroke * 2), 0.0, 0.0), rot);

	child[(w * 2) + 1] = nullptr;

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	glm::vec3 origin = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = origin[i];
	}

	return _;
}

Truck* truckMk(char* data, unsigned int no, glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_ptr = data;
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

	child[w * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(no * layout::idx[Y]) - (layout::stroke) - (layout::stroke * 2), 0.0, 0.0), rot);

	Obj* bed = ptMk(_->_vtx, "bevel/main", "bed", "dir", glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	bed->_prog.use();

	GLuint uniSzBed = glGetUniformLocation(bed->_prog._id, "sz");

	glUniform1ui(uniSzBed, no);

	bed->_prog.unUse();

	Obj* outer = ptMk(_->_vtx, "bevel/main", "outer", "dir", glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	outer->_prog.use();

	GLuint uniSzOuter = glGetUniformLocation(outer->_prog._id, "sz");

	glUniform1ui(uniSzOuter, no);

	outer->_prog.unUse();

	pt.push_back(bed);
	pt.push_back(outer);

	child[(w * 2) + 1] = bed;
	child[(w * 2) + 1 + 1] = outer;

	_->_data = arrMk(data, no, "", glm::vec3(0.0, 0.0, -((layout::idx[X] / 2) + (layout::stroke * 2) + (layout::margin * 2 * 2))), glm::vec3(0.0, -M_PI / 2, 0.0));

	child[(w * 2) + 1 + 1] = _->_data->_parent;

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	glm::vec3 origin = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = origin[i];
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

	glm::vec3 origin = truck->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		truck->_loc[i] = origin[i];
	}
}

void truckMv(Truck* truck, float delta) {
	glm::vec3 loc = glm::vec3(delta, 0.0, 0.0);

	objAnim(truck->_parent, loc, glm::vec3(0.0));

	glm::vec3 origin = truck->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		truck->_loc[i] = origin[i];
	}
}
