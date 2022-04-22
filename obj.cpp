#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include <SDL2/SDL_image.h>
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
#include "layout.h"
#include "stb_image.h"

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noUni = 5;
	_->_uni = (GLint*) malloc(_->_noUni * sizeof (GLint));
	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;
	_->_tex = 0;

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			_->_aabb[a][b] = 0.0;
		}
	}

	_->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
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

	glUniform1ui(_->_uni[Obj::T], disp->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	util::mesh::aabb(_->_aabb, vtc, idc, noPrim);

	// rig
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noUni = 5;
	_->_uni = (GLint*) malloc(_->_noUni * sizeof (GLint));
	_->_active = active;
	_->_noChild = noChild;
	_->_child = (Obj**) malloc(_->_noChild * sizeof (Obj*));
	for (int i = 0; i < _->_noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_v = 0.0;
	_->_tex = 0;

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			_->_aabb[a][b] = 0.0;
		}
	}

	_->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
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

	glUniform1ui(_->_uni[Obj::T], disp->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	util::mesh::aabb(_->_aabb, vtc, idc, noPrim);

	// rig
	objAcc(_, glm::mat4(1.0));

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name, Obj::POS);

	Obj* _ = objMk(&vtc[0], &idc[0], idc.size(), vtx, frag, active, loc, rot);

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idc = util::mesh::rd::idc(name, Obj::POS);

	Obj* _ = objMk(&vtc[0], &idc[0], idc.size(), vtx, frag, active, child, noChild, loc, rot);

	return _;
}

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, std::string tex, bool active, glm::vec3 loc, glm::vec3 rot) {
	Obj* _ = objMk(vtc, idc, noPrim, vtx, frag, active, loc, rot);

	glBindVertexArray(_->_mesh->_id[Mesh::VAO]);

	glGenBuffers(1, &_->_mesh->_id[Mesh::STBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 2 * sizeof (GLfloat), st, GL_STATIC_DRAW);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	_->_attr[Obj::ST] = glGetAttribLocation(_->_prog._id, "st");
	glVertexAttribPointer(_->_attr[Obj::ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::ST]);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, std::string tex, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	Obj* _ = objMk(vtc, idc, noPrim, vtx, frag, active, child, noChild, loc, rot);

	glBindVertexArray(_->_mesh->_id[Mesh::VAO]);

	glGenBuffers(1, &_->_mesh->_id[Mesh::STBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 2 * sizeof (GLfloat), st, GL_STATIC_DRAW);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[Mesh::STBO]);
	_->_attr[Obj::ST] = glGetAttribLocation(_->_prog._id, "st");
	glVertexAttribPointer(_->_attr[Obj::ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[Obj::ST]);

	// texture
	glGenTextures(1, &_->_tex);
	glBindTexture(GL_TEXTURE_2D, _->_tex);

	int dim[2];
	int chan;
	unsigned char* data = stbi_load(util::fs::path::build({
		"res",
		tex
	}).c_str(), &dim[X], &dim[Y], &chan, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim[X], dim[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, std::string tex, bool active, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idcVtc = util::mesh::rd::idc(name, Obj::POS);

	std::vector<GLfloat> st = util::mesh::rd::st(name);
	std::vector<GLushort> idcSt = util::mesh::rd::idc(name, Obj::ST);

	std::vector<GLfloat> stIdxed(st.size());
	for (int i = 0; i < idcVtc.size(); i++) {
		int idxPos = idcVtc[i] * 2;
		int idxSt = idcSt[i] * 2;

		for (int a = 0; a < 2; a++) {
			stIdxed[idxPos + a] = st[idxSt + a];
		}
	}

	Obj* _ = objMk(&vtc[0], &stIdxed[0], &idcVtc[0], idcVtc.size(), vtx, frag, tex, active, loc, rot);

	// texture
	glGenTextures(1, &_->_tex);
	glBindTexture(GL_TEXTURE_2D, _->_tex);

	int dim[2];
	int chan;
	unsigned char* data = stbi_load(util::fs::path::build({
		"res",
		tex
	}).c_str(), &dim[X], &dim[Y], &chan, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim[X], dim[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, std::string tex, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::vtc(name);
	std::vector<GLushort> idcVtc = util::mesh::rd::idc(name, Obj::POS);

	std::vector<GLfloat> st = util::mesh::rd::st(name);
	std::vector<GLushort> idcSt = util::mesh::rd::idc(name, Obj::ST);

	std::vector<GLfloat> stIdxed(st.size());
	for (int i = 0; i < idcVtc.size(); i++) {
		int idxPos = idcVtc[i] * 2;
		int idxSt = idcSt[i] * 2;

		for (int a = 0; a < 2; a++) {
			stIdxed[idxPos + a] = st[idxSt + a];
		}
	}

	Obj* _ = objMk(&vtc[0], &stIdxed[0], &idcVtc[0], idcVtc.size(), vtx, frag, tex, active, child, noChild, loc, rot);

	return _;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noUni = 5;
	_->_uni = (GLint*) malloc(_->_noUni * sizeof (GLint));
	_->_active = active;
	_->_child = nullptr;
	_->_noChild = 0;
	_->_v = 0.0;
	_->_tex = 0;

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			_->_aabb[a][b] = 0.0;
		}
	}

	_->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= glm::translate(glm::mat4(1.0), loc);
	_->_model *= util::matr::rot(glm::mat4(1.0), rot);
	_->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
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

	glUniform1ui(_->_uni[Obj::T], disp->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	// rig
	objAcc(_, glm::mat4(1.0));

	return _;
}

void objDel(Obj* obj) {
	meshDel(obj->_mesh);

	free(obj->_uni);

	for (int i = 0; i < obj->_noChild; i++) {
		objDel(obj->_child[i]);
	}
	free(obj->_child);

	free(obj);
}

void objAcc(Obj* obj, glm::mat4 prev) {
	obj->_acc = prev * obj->_model;

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objAcc(obj->_child[i], obj->_acc);
		}
	}
}

void objAnim(Obj* obj, Obj* parent, glm::vec3 loc, glm::vec3 rot, GLfloat speed) {
	glm::vec3 locMax = glm::abs(loc);
	glm::vec3 rotMax = glm::abs(rot);

	glm::vec3 locInc = (loc / glm::vec3(state::fps));
	if (loc != glm::vec3(0.0)) {
		locInc = glm::normalize(locInc);
	}

	locInc *= glm::vec3(state::speed);
	locInc *= glm::vec3(speed);

	glm::vec3 rotInc = (rot / glm::vec3(state::fps));
	if (rot != glm::vec3(0.0)) {
		rotInc = glm::normalize(rotInc);
	}

	rotInc *= glm::vec3(state::speed);
	rotInc *= glm::vec3(speed);

	glm::vec3 locFrame = glm::vec3(0.0);
	glm::vec3 rotFrame = glm::vec3(0.0);
	if (state::anim) {
		while (glm::any(glm::lessThan(locFrame, locMax)) || glm::any(glm::lessThan(rotFrame, rotMax))) {
			glm::vec3 stepLoc;
			if (glm::any(glm::lessThan(locFrame + locInc, locMax))) {
				stepLoc = locInc;
			} else  {
				stepLoc = loc - locFrame;
			}

			glm::vec3 stepRot;
			if (glm::any(glm::lessThan(rotFrame + rotInc, rotMax))) {
				stepRot = rotInc;
			} else  {
				stepRot = rot - rotFrame;
			}

			glm::mat4 trans = glm::mat4(1.0);
			trans = glm::translate(trans, stepLoc);
			trans = util::matr::rot(trans, stepRot);

			obj->_model *= trans;

			glm::mat4 prev;
			if (parent) {
				prev = parent->_acc;
			} else {
				prev = glm::mat4(1.0);
			}

			objAcc(obj, prev);

			locFrame += glm::abs(stepLoc);
			rotFrame += glm::abs(stepRot);

			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));
		}
	} else {
		glm::mat4 trans = glm::mat4(1.0);
		trans = glm::translate(trans, loc);
		trans = util::matr::rot(trans, rot);

		objAcc(obj, obj->_acc * trans);
	}
}

void objMv(Obj* obj, Obj* parent, glm::vec3 loc, glm::vec3 rot, GLfloat speed) {
	bool coll = false;
	glm::mat4 dest = glm::mat4(1.0);
	dest = glm::translate(dest, loc);
	dest = util::matr::rot(dest, rot);
	for (int i = 0; i < scn::obj.size(); i++) {
		if (obj != scn::obj[i]) {
			if (util::phys::aabb(obj, scn::obj[i])) {
				coll = true;

				break;
			}
		}
	}

	if (!coll) {
		objAnim(obj, parent, loc, rot, speed);
	} else {
		omni::err(omni::ERR_OBJ_CLIP);
	}
}

void objA(Obj* obj) {
	int i = 0;
	while (!util::phys::aabbGround(obj)) {
		objMv(obj, nullptr, glm::vec3(0.0, -(pow(i, 2) * phys::g), 0.0), glm::vec3(0.0));

		i++;
	}
}

void objDraw(Obj* obj, unsigned int type) {
	GLenum prim;
	switch (type) {
		case Mesh::PT:
			prim = GL_POINTS;

			break;

		case Mesh::LINE:
			prim = GL_LINES;

			break;

		case Mesh::OBJ:
			prim = GL_TRIANGLES;

			break;
	};

	obj->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	obj->_view = glm::scale(obj->_view, cam._scale);

	glBindVertexArray(obj->_mesh->_id[Mesh::VAO]);
	obj->_prog.use();
	glBindTexture(GL_TEXTURE_2D, obj->_tex);

	glUniformMatrix4fv(obj->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(obj->_acc));
	glUniformMatrix4fv(obj->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));

	glUniform1ui(obj->_uni[Obj::T], disp->_t);

	glUniform1ui(obj->_uni[Obj::ACTIVE], obj->_active);

	glDrawElements(prim, obj->_mesh->_noPrim, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	obj->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objDraw(obj->_child[i], type);
		}
	}
}
