#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj.h"
#include "scn.h"
#include "util.h"
#include "state.h"
#include "math.h"
#include "state.h"
#include "phys.h"

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;
	_->_active = active;
	_->_noChild = 0;
	_->_loc = loc;
	_->_rot = rot;
	_->_v = 0.0;
	_->_t = 0;

	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, noIdc * sizeof (GLushort), idc, GL_STATIC_DRAW);

	// matrix
	_->_proj = glm::ortho(-(state::view[X] / 2.0f), state::view[X] / 2.0f, -(state::view[Y] / 2.0f), state::view[Y] / 2.0f, 0.1f, 10000.0f);
	_->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, cam._scale);
	_->_model = glm::translate(_->_model, _->_loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		_->_model = glm::rotate(_->_model, _->_rot[i], axis);
	}

	_->_prog = Prog(nameVtx, nameFrag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;
	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_loc = loc;
	_->_rot = rot;
	_->_v = 0.0;
	_->_t = 0;

	// vertex
	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, noIdc * sizeof (GLushort), idc, GL_STATIC_DRAW);

	// matrix
	_->_proj = glm::ortho(-(state::view[X] / 2.0f), state::view[X] / 2.0f, -(state::view[Y] / 2.0f), state::view[Y] / 2.0f, 0.1f, 10000.0f);
	_->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, cam._scale);
	_->_model = glm::translate(_->_model, _->_loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		_->_model = glm::rotate(_->_model, _->_rot[i], axis);
	}

	_->_prog = Prog(nameVtx, nameFrag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;
	_->_active = active;
	_->_noChild = 0;
	_->_loc = loc;
	_->_rot = rot;
	_->_v = 0.0;
	_->_t = 0;

	// vertex
	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, noIdc * sizeof (GLushort), idc, GL_STATIC_DRAW);

	// matrix
	_->_proj = glm::ortho(-(state::view[X] / 2.0f), state::view[X] / 2.0f, -(state::view[Y] / 2.0f), state::view[Y] / 2.0f, 0.1f, 10000.0f);
	_->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, cam._scale);
	_->_model = glm::translate(_->_model, _->_loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		_->_model = glm::rotate(_->_model, _->_rot[i], axis);
	}

	_->_prog = Prog(nameVtx, nameGeom, nameFrag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;
	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_loc = loc;
	_->_rot = rot;
	_->_v = 0.0;
	_->_t = 0;

	// vertex
	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, noIdc * sizeof (GLushort), idc, GL_STATIC_DRAW);

	// matrix
	_->_proj = glm::ortho(-(state::view[X] / 2.0f), state::view[X] / 2.0f, -(state::view[Y] / 2.0f), state::view[Y] / 2.0f, 0.1f, 10000.0f);
	_->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, cam._scale);
	_->_model = glm::translate(_->_model, _->_loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		_->_model = glm::rotate(_->_model, _->_rot[i], axis);
	}

	_->_prog = Prog(nameVtx, nameGeom, nameFrag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_noChild = 0;
	_->_loc = loc;
	_->_rot = rot;
	_->_v = 0.0;
	_->_t = 0;

	// vertex
	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	glBufferData(GL_ARRAY_BUFFER, vtc.size() * sizeof (GLfloat), &vtc[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	std::vector<GLushort> idc = util::mesh::rd::idc(name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idc.size() * sizeof (GLushort), &idc[0], GL_STATIC_DRAW);

	_->_noIdc = idc.size();

	// matrix
	_->_proj = glm::ortho(-(state::view[X] / 2.0f), state::view[X] / 2.0f, -(state::view[Y] / 2.0f), state::view[Y] / 2.0f, 0.1f, 10000.0f);
	_->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, cam._scale);
	_->_model = glm::translate(_->_model, _->_loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		_->_model = glm::rotate(_->_model, _->_rot[i], axis);
	}

	_->_prog = Prog(nameVtx, nameFrag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_loc = loc;
	_->_rot = rot;
	_->_v = 0.0;
	_->_t = 0;

	// vertex
	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	glBufferData(GL_ARRAY_BUFFER, vtc.size() * sizeof (GLfloat), &vtc[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	std::vector<GLushort> idc = util::mesh::rd::idc(name);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idc.size() * sizeof (GLushort), &idc[0], GL_STATIC_DRAW);

	_->_noIdc = idc.size();

	// matrix
	_->_proj = glm::ortho(-(state::view[X] / 2.0f), state::view[X] / 2.0f, -(state::view[Y] / 2.0f), state::view[Y] / 2.0f, 0.1f, 10000.0f);
	_->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::translate(_->_model, _->_loc);
	_->_model = glm::scale(_->_model, cam._scale);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		_->_model = glm::rotate(_->_model, _->_rot[i], axis);
	}

	_->_prog = Prog(nameVtx, nameFrag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

void anim(Obj* obj, GLfloat* d) {
	for (int t = 0; t < state::fps; t++) {
		for (int i = 0; i < 3; i++) {
			obj->_loc[i] += d[i] / state::fps;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));
	}

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			anim(obj->_child[i], d);
		}
	}
}

void objMv(Obj* obj, GLfloat* d) {
	anim(obj, d);
}

void objA(Obj* obj) {
	while (!util::phys::aabb(obj, Y, 0.0, glm::vec3(obj->_loc[X], obj->_loc[Y] - obj->_v, obj->_loc[Z]))) {
		obj->_v -= (phys::g / state::fps);

		obj->_loc[Y] += obj->_v;
	}
}

void objDraw(Obj* obj) {
	obj->_view = glm::lookAt(cam._pos + glm::vec3(100, 100, 100), cam._pos, glm::vec3(0, 1, 0));

	obj->_model = glm::mat4(1.0);
	obj->_model = glm::scale(obj->_model, cam._scale);
	obj->_model = glm::translate(obj->_model, obj->_loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		obj->_model = glm::rotate(obj->_model, obj->_rot[i], axis);
	}

	glBindVertexArray(obj->_id[VAO]);
	obj->_prog.use();

	glUniformMatrix4fv(obj->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(obj->_model));
	glUniformMatrix4fv(obj->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));

	glUniform1ui(obj->_uni[T], obj->_t);

	glUniform1ui(obj->_uni[ACTIVE], obj->_active);

	glDrawElements(GL_TRIANGLES, obj->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);

	obj->_t++;

	for (int i = 0; i < obj->_noChild; i++) {
		if (&obj->_child[i]) {
			objDraw(obj->_child[i]);
		}
	}
}
