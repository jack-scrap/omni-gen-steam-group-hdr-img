#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "obj.h"
#include "scn.h"
#include "util.h"
#include "state.h"
#include "math.h"
#include "state.h"
#include "phys.h"
#include "omni.h"

#include "stb_image.h"

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_uni = (GLint*) malloc(5 * sizeof (GLint));
	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;

	_->_mesh = meshMk(vtc, noPrim * 3, idc, noPrim);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::VBO]);
	_->_attr[Obj::POS] = glGetAttribLocation(_->_prog._id, "pos");
	glVertexAttribPointer(_->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::POS]);

	// uniform
	_->_uni[Obj::MODEL] = glGetUniformLocation(_->_prog._id, "model");
	_->_uni[Obj::VIEW] = glGetUniformLocation(_->_prog._id, "view");
	_->_uni[Obj::PROJ] = glGetUniformLocation(_->_prog._id, "proj");

	_->_uni[Obj::ACTIVE] = glGetUniformLocation(_->_prog._id, "active");

	_->_uni[Obj::T] = glGetUniformLocation(_->_prog._id, "t");

	glUniformMatrix4fv(_->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[Obj::ACTIVE], _->_active);

	glUniform1ui(_->_uni[Obj::T], scn::_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_bound[i][b] = 0.0;
		}
	}

	util::mesh::aabb(_->_bound, vtc, noPrim * 3);

	// rig
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_uni = (GLint*) malloc(5 * sizeof (GLint));
	_->_active = active;
	_->_noChild = noChild;
	_->_child = (Obj**) malloc(_->_noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_v = 0.0;

	_->_mesh = meshMk(vtc, noPrim * 3, idc, noPrim);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::VBO]);
	_->_attr[Obj::POS] = glGetAttribLocation(_->_prog._id, "pos");
	glVertexAttribPointer(_->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::POS]);

	// uniform
	_->_uni[Obj::MODEL] = glGetUniformLocation(_->_prog._id, "model");
	_->_uni[Obj::VIEW] = glGetUniformLocation(_->_prog._id, "view");
	_->_uni[Obj::PROJ] = glGetUniformLocation(_->_prog._id, "proj");

	_->_uni[Obj::ACTIVE] = glGetUniformLocation(_->_prog._id, "active");

	_->_uni[Obj::T] = glGetUniformLocation(_->_prog._id, "t");

	glUniformMatrix4fv(_->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[Obj::ACTIVE], _->_active);

	glUniform1ui(_->_uni[Obj::T], scn::_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	for (int i = 0; i < 3; i++) {
		for (int b = 0; b < 2; b++) {
			_->_bound[i][b] = 0.0;
		}
	}

	util::mesh::aabb(_->_bound, vtc, noPrim * 3);

	// rig
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name, 0);

	Obj* _ = objMk(&vtc[0], &idc[0], idc.size(), vtx, frag, active, loc, rot);

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name, 0);

	Obj* _ = objMk(&vtc[0], &idc[0], idc.size(), vtx, frag, active, child, noChild, loc, rot);

	return _;
}

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	Obj* _ = objMk(vtc, idc, noPrim, vtx, frag, active, loc, rot);

	// texture
	int
		wd,
		ht,
		chan;
	GLubyte* data = stbi_load("res/dirt.jpg", &wd, &ht, &chan, 0);

	if (data) {
		glGenTextures(1, &_->_tex);
		glBindTexture(GL_TEXTURE_2D, _->_tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wd, ht, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		omni::err("Texture failed to load");
	}

	glBindVertexArray(_->_mesh->_id[Mesh::VAO]);

	glGenBuffers(1, &_->_mesh->_id[Mesh::STBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 2 * sizeof (GLfloat), st, GL_STATIC_DRAW);

	glBindVertexArray(0);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	_->_attr[Obj::ST] = glGetAttribLocation(_->_prog._id, "st");
	glVertexAttribPointer(_->_attr[Obj::ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::ST]);

	_->_prog.unUse();

	return _;
}

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	Obj* _ = objMk(vtc, idc, noPrim, vtx, frag, active, child, noChild, loc, rot);

	// texture
	int
		wd,
		ht,
		chan;
	GLubyte* data = stbi_load("res/dirt.jpg", &wd, &ht, &chan, 0);

	if (data) {
		glGenTextures(1, &_->_tex);
		glBindTexture(GL_TEXTURE_2D, _->_tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wd, ht, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		omni::err("Texture failed to load");
	}

	glBindVertexArray(_->_mesh->_id[Mesh::VAO]);

	glGenBuffers(1, &_->_mesh->_id[Mesh::STBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 2 * sizeof (GLfloat), st, GL_STATIC_DRAW);

	glBindVertexArray(0);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	_->_attr[Obj::ST] = glGetAttribLocation(_->_prog._id, "st");
	glVertexAttribPointer(_->_attr[Obj::ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::ST]);

	_->_prog.unUse();

	return _;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_uni = (GLint*) malloc(5 * sizeof (GLint));
	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;

	_->_mesh = meshMk(vtc, noPrim * 3, idc, noPrim);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 10000.0);
	_->_acc = glm::mat4(1.0);

	_->_prog = Prog(vtx, geom, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::VBO]);
	_->_attr[Obj::POS] = glGetAttribLocation(_->_prog._id, "pos");
	glVertexAttribPointer(_->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::POS]);

	// uniform
	_->_uni[Obj::MODEL] = glGetUniformLocation(_->_prog._id, "model");
	_->_uni[Obj::VIEW] = glGetUniformLocation(_->_prog._id, "view");
	_->_uni[Obj::PROJ] = glGetUniformLocation(_->_prog._id, "proj");

	_->_uni[Obj::ACTIVE] = glGetUniformLocation(_->_prog._id, "active");

	_->_uni[Obj::T] = glGetUniformLocation(_->_prog._id, "t");

	glUniformMatrix4fv(_->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[Obj::ACTIVE], _->_active);

	glUniform1ui(_->_uni[Obj::T], scn::_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// rig
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
	glm::vec3
		locMax = glm::abs(loc),
		rotMax = glm::abs(rot),

		locInc = (loc / glm::vec3(state::fps)) * glm::vec3(state::speed),
		rotInc = (rot / glm::vec3(state::fps)) * glm::vec3(state::speed),

		locFrame = glm::vec3(0.0),
		rotFrame = glm::vec3(0.0);

	while (
		glm::any(glm::lessThan(locFrame, locMax)) ||
		glm::any(glm::lessThan(rotFrame, rotMax))
	) {
		glm::mat4 trans = glm::mat4(1.0);
		trans = glm::translate(trans, locInc);
		trans = util::matr::rot(trans, rotInc);

		objAcc(obj, obj->_acc * trans);

		locFrame += glm::abs(locInc);
		rotFrame += glm::abs(rotInc);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));
	}
}

void objTrans(Obj* obj, glm::vec3 loc, glm::vec3 rot) {
	objAnim(obj, loc, rot);
}

void objA(Obj* obj) {
	int i = 0;
	while (!util::phys::aabbGround(obj)) {
		objTrans(obj, glm::vec3(0.0, -(pow(i, 2) * phys::g), 0.0), glm::vec3(0.0));

		i++;
	}
}

void objDraw(Obj* obj) {
	obj->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	obj->_view = glm::scale(obj->_view, cam._scale);

	glBindVertexArray(obj->_mesh->_id[Mesh::VAO]);
	obj->_prog.use();

	glBindTexture(GL_TEXTURE_2D, obj->_tex);

	glUniformMatrix4fv(obj->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(obj->_acc));
	glUniformMatrix4fv(obj->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));

	glUniform1ui(obj->_uni[Obj::T], scn::_t);

	glUniform1ui(obj->_uni[Obj::ACTIVE], obj->_active);

	glDrawElements(GL_TRIANGLES, obj->_mesh->_noPrim, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objDraw(obj->_child[i]);
		}
	}
}
