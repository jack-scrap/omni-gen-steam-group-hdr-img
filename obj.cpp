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
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

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
	util::mesh::bound(_->_bound, vtc, noVtc);

	// children
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i]->_model *= _->_loc;
		_->_child[i]->_model *= _->_rot;
	}

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_v = 0.0;
	_->_t = 0;
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

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
	util::mesh::bound(_->_bound, vtc, noVtc);

	// children
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i]->_model *= _->_loc;
		_->_child[i]->_model *= _->_rot;
	}

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
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

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
	util::mesh::bound(_->_bound, vtc, noVtc);

	// children
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i]->_model *= _->_loc;
		_->_child[i]->_model *= _->_rot;
	}

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string geom, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_v = 0.0;
	_->_t = 0;
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	_->_mesh = meshMk(vtc, noVtc, idc, noIdc);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

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
	util::mesh::bound(_->_bound, vtc, noVtc);

	// children
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i]->_model *= _->_loc;
		_->_child[i]->_model *= _->_rot;
	}

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
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name);
	_->_mesh = meshMk(&vtc[0], vtc.size(), &idc[0], idc.size());

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

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
	util::mesh::bound(_->_bound, &vtc[0], vtc.size());

	// children
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i]->_model *= _->_loc;
		_->_child[i]->_model *= _->_rot;
	}

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_v = 0.0;
	_->_t = 0;
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name);
	_->_mesh = meshMk(&vtc[0], vtc.size(), &idc[0], idc.size());

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

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
	util::mesh::bound(_->_bound, &vtc[0], vtc.size());

	// children
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i]->_model *= _->_loc;
		_->_child[i]->_model *= _->_rot;
	}

	return _;
}

void objUpdate(Obj* obj, glm::vec3 loc, glm::vec3 rot) {
	obj->_model = glm::translate(obj->_model, loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		obj->_model = glm::rotate(obj->_model, rot[i], axis);
	}

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			glm::mat4 tmp = glm::mat4(1.0);
			tmp *= obj->_child[i]->_loc;
			tmp *= obj->_child[i]->_rot;

			obj->_child[i]->_model = obj->_model;
			obj->_child[i]->_model *= tmp;

			objUpdate(obj->_child[i], glm::vec3(0.0), glm::vec3(0.0));
		}
	}
}

void objAnim(Obj* obj, glm::vec3 loc, glm::vec3 rot) {
	float max = 0.0;
	for (int i = 0; i < 3; i++) {
		if (loc[i] > max) {
			max = loc[i];
		}
	}

	float end = max * state::fps;

	int t = 0;
	while (t < end) {
		objUpdate(obj, loc / glm::vec3(end), rot / glm::vec3(end));

		t++;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));
	}
}

void objMv(Obj* obj, GLfloat* loc) {
	std::thread t(objAnim, obj, glm::vec3(loc[0], loc[1], loc[2]), glm::vec3(0.0));
	t.detach();
}

void objA(Obj* obj) {
}

void objDraw(Obj* obj) {
	obj->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(100.0, -100.0, 100.0), glm::vec3(0, 1, 0));
	obj->_view = glm::scale(obj->_view, cam._scale);

	glBindVertexArray(obj->_mesh->_id[VAO]);
	obj->_prog.use();

	glUniformMatrix4fv(obj->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(obj->_model));
	glUniformMatrix4fv(obj->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));

	glUniform1ui(obj->_uni[T], obj->_t);

	glUniform1ui(obj->_uni[ACTIVE], obj->_active);

	glDrawElements(GL_TRIANGLES, obj->_mesh->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);

	obj->_t++;

	for (int i = 0; i < obj->_noChild; i++) {
		if (&obj->_child[i]) {
			objDraw(obj->_child[i]);
		}
	}
}
