#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "obj.h"
#include "scn.h"
#include "util.h"
#include "state.h"
#include "math.h"
#include "state.h"
#include "phys.h"

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;
	_->_t = 0;

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(_->_rng, vtc, noVtc);

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				_->_bound[i] = _->_rng[X][x];
				_->_bound[i + 1] = _->_rng[Y][y];
				_->_bound[i + 2] = _->_rng[Z][z];

				i += 3;
			}
		}
	}

	// children
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	_->_noChild = noChild;
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_v = 0.0;
	_->_t = 0;

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(_->_rng, vtc, noVtc);

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				_->_bound[i] = _->_rng[X][x];
				_->_bound[i + 1] = _->_rng[Y][y];
				_->_bound[i + 2] = _->_rng[Z][z];

				i += 3;
			}
		}
	}

	// children
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;
	_->_t = 0;

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, geom, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(_->_rng, vtc, noVtc);

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				_->_bound[i] = _->_rng[X][x];
				_->_bound[i + 1] = _->_rng[Y][y];
				_->_bound[i + 2] = _->_rng[Z][z];

				i += 3;
			}
		}
	}

	// children
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string geom, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_noChild = noChild;
	_->_child = (Obj**) malloc(_->_noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_v = 0.0;
	_->_t = 0;

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, geom, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(_->_rng, vtc, noVtc);

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				_->_bound[i] = _->_rng[X][x];
				_->_bound[i + 1] = _->_rng[Y][y];
				_->_bound[i + 2] = _->_rng[Z][z];

				i += 3;
			}
		}
	}

	// children
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;
	_->_t = 0;

	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name);
	_->_mesh = meshMk(&vtc[0], vtc.size(), &idc[0], idc.size());

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(_->_rng, &vtc[0], vtc.size());

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				_->_bound[i] = _->_rng[X][x];
				_->_bound[i + 1] = _->_rng[Y][y];
				_->_bound[i + 2] = _->_rng[Z][z];

				i += 3;
			}
		}
	}

	// children
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	_->_noChild = noChild;
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_v = 0.0;
	_->_t = 0;

	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name);
	_->_mesh = meshMk(&vtc[0], vtc.size(), &idc[0], idc.size());

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_rng[i][b] = 0.0;
		}
	}

	util::mesh::bound(_->_rng, &vtc[0], vtc.size());

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				_->_bound[i] = _->_rng[X][x];
				_->_bound[i + 1] = _->_rng[Y][y];
				_->_bound[i + 2] = _->_rng[Z][z];

				i += 3;
			}
		}
	}

	// children
	objAcc(_, glm::mat4(1.0));

	return _;
}

void objAcc(Obj* obj, glm::mat4 prev) {
	obj->_acc = prev * obj->_model;

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objAcc(obj->_child[i], obj->_acc);
		}
	}
}

void objAnim(Obj* obj, glm::vec3 loc, glm::vec3 rot) {
	glm::vec3 locInc = loc / glm::vec3(state::fps);
	glm::vec3 rotInc = rot / glm::vec3(state::fps);

	glm::vec3 locMax = glm::abs(loc);
	glm::vec3 rotMax = glm::abs(rot);

	glm::vec3 locFrame = glm::vec3(0.0);
	glm::vec3 rotFrame = glm::vec3(0.0);
	while (
		glm::any(glm::lessThan(locFrame, locMax)) ||
		glm::any(glm::lessThan(rotFrame, rotMax))
	) {
		if (util::phys::aabb(obj, mesh[1])) {
			break;
		}

		glm::mat4 d = glm::mat4(1.0);
		d = glm::translate(d, locInc);
		d = util::matr::rot(d, rotInc);

		obj->_model *= d;
		obj->_acc *= d;

		locFrame += glm::abs(locInc);
		rotFrame += glm::abs(rotInc);

		for (int i = 0; i < obj->_noChild; i++) {
			if (obj->_child[i]) {
				objAcc(obj->_child[i], obj->_acc);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));
	}
}

void objTrans(Obj* obj, glm::vec3 loc, glm::vec3 rot) {
	objAnim(obj, loc, rot);
}

void objA(Obj* obj) {
}

void objDraw(Obj* obj) {
	obj->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	obj->_view = glm::scale(obj->_view, cam._scale);

	glBindVertexArray(obj->_mesh->_id[VAO]);
	obj->_prog.use();

	glUniformMatrix4fv(obj->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(obj->_acc));
	glUniformMatrix4fv(obj->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));

	glUniform1ui(obj->_uni[T], obj->_t);

	glUniform1ui(obj->_uni[ACTIVE], obj->_active);

	glDrawElements(GL_TRIANGLES, obj->_mesh->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);

	obj->_t++;
	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objDraw(obj->_child[i]);
		}
	}
}
