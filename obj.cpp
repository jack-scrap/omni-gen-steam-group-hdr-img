#define GLM_ENABLE_EXPERIMENTAL

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

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int type, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* inst = (Obj*) malloc(sizeof (Obj));

	inst->_noUni = 5;
	inst->_uni = (GLint*) malloc(inst->_noUni * sizeof (GLint));
	inst->_active = active;
	inst->_type = type;
	inst->_child = nullptr;
	inst->_noChild = 0;
	inst->_v = 0.0;
	inst->_tex = 0;

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			inst->_aabb[a][b] = 0.0;
		}
	}

	inst->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	inst->_model = glm::mat4(1.0);
	inst->_model *= glm::translate(glm::mat4(1.0), loc);
	inst->_model *= util::matr::rot(glm::mat4(1.0), rot);
	inst->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	inst->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
	inst->_acc = glm::mat4(1.0);

	inst->_prog = Prog(vtx, frag);

	inst->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::VBO]);
	inst->_attr[Obj::POS] = glGetAttribLocation(inst->_prog._id, "pos");
	glVertexAttribPointer(inst->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(inst->_attr[Obj::POS]);

	// uniform
	inst->_uni[Obj::MODEL] = glGetUniformLocation(inst->_prog._id, "model");
	inst->_uni[Obj::VIEW] = glGetUniformLocation(inst->_prog._id, "view");
	inst->_uni[Obj::PROJ] = glGetUniformLocation(inst->_prog._id, "proj");

	inst->_uni[Obj::ACTIVE] = glGetUniformLocation(inst->_prog._id, "active");

	inst->_uni[Obj::T] = glGetUniformLocation(inst->_prog._id, "t");

	glUniformMatrix4fv(inst->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(inst->_model));
	glUniformMatrix4fv(inst->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(inst->_view));
	glUniformMatrix4fv(inst->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(inst->_proj));

	glUniform1ui(inst->_uni[Obj::ACTIVE], inst->_active);

	glUniform1ui(inst->_uni[Obj::T], disp->t);

	inst->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	util::mesh::aabb(inst->_aabb, vtc, idc, noPrim);

	// rig
	objAcc(inst, glm::mat4(1.0));

	return inst;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int type, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* inst = (Obj*) malloc(sizeof (Obj));

	inst->_noUni = 5;
	inst->_uni = (GLint*) malloc(inst->_noUni * sizeof (GLint));
	inst->_active = active;
	inst->_type = type;
	inst->_noChild = noChild;
	inst->_child = (Obj**) malloc(inst->_noChild * sizeof (Obj*));
	for (int i = 0; i < inst->_noChild; i++) {
		inst->_child[i] = child[i];
	}
	inst->_v = 0.0;
	inst->_tex = 0;

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			inst->_aabb[a][b] = 0.0;
		}
	}

	inst->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	inst->_model = glm::mat4(1.0);
	inst->_model *= glm::translate(glm::mat4(1.0), loc);
	inst->_model *= util::matr::rot(glm::mat4(1.0), rot);
	inst->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	inst->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
	inst->_acc = glm::mat4(1.0);

	inst->_prog = Prog(vtx, frag);

	inst->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::VBO]);
	inst->_attr[Obj::POS] = glGetAttribLocation(inst->_prog._id, "pos");
	glVertexAttribPointer(inst->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(inst->_attr[Obj::POS]);

	// uniform
	inst->_uni[Obj::MODEL] = glGetUniformLocation(inst->_prog._id, "model");
	inst->_uni[Obj::VIEW] = glGetUniformLocation(inst->_prog._id, "view");
	inst->_uni[Obj::PROJ] = glGetUniformLocation(inst->_prog._id, "proj");

	inst->_uni[Obj::ACTIVE] = glGetUniformLocation(inst->_prog._id, "active");

	inst->_uni[Obj::T] = glGetUniformLocation(inst->_prog._id, "t");

	glUniformMatrix4fv(inst->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(inst->_model));
	glUniformMatrix4fv(inst->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(inst->_view));
	glUniformMatrix4fv(inst->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(inst->_proj));

	glUniform1ui(inst->_uni[Obj::ACTIVE], inst->_active);

	glUniform1ui(inst->_uni[Obj::T], disp->t);

	inst->_prog.unUse();
	glBindVertexArray(0);

	// bounds
	util::mesh::aabb(inst->_aabb, vtc, idc, noPrim);

	// rig
	objAcc(inst, glm::mat4(1.0));

	return inst;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::attr(name, Obj::POS);
	std::vector<GLushort> idc = util::mesh::rd::idc(name, Obj::POS);

	return objMk(&vtc[0], &idc[0], idc.size(), Mesh::OBJ, vtx, frag, active, loc, rot);
}

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtc = util::mesh::rd::attr(name, Obj::POS);
	std::vector<GLushort> idc = util::mesh::rd::idc(name, Obj::POS);

	return objMk(&vtc[0], &idc[0], idc.size(), Mesh::OBJ, vtx, frag, active, child, noChild, loc, rot);
}

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, unsigned int type, std::string vtx, std::string frag, std::string tex, bool active, glm::vec3 loc, glm::vec3 rot) {
	Obj* inst = objMk(vtc, idc, noPrim, type, vtx, frag, active, loc, rot);

	glBindVertexArray(inst->_mesh->_id[Mesh::VAO]);

	glGenBuffers(1, &inst->_mesh->_id[Mesh::STBO]);
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::STBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 2 * sizeof (GLfloat), st, GL_STATIC_DRAW);

	inst->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::STBO]);
	inst->_attr[Obj::ST] = glGetAttribLocation(inst->_prog._id, "st");
	glVertexAttribPointer(inst->_attr[Obj::ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(inst->_attr[Obj::ST]);

	// texture
	inst->_tex = util::tex::rd(tex);

	inst->_prog.unUse();
	glBindVertexArray(0);

	return inst;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, std::string tex, bool active, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtcRaw = util::mesh::rd::attr(name, Obj::POS);
	std::vector<GLushort> idcVtc = util::mesh::rd::idc(name, Obj::POS);

	std::vector<GLfloat> stRaw = util::mesh::rd::attr(name, Obj::ST);
	std::vector<GLushort> idcSt = util::mesh::rd::idc(name, Obj::ST);

	std::vector<GLfloat> vtc;
	for (int i = 0; i < idcVtc.size(); i++) {
		int idx = idcVtc[i];

		for (int a = 0; a < 3; a++) {
			vtc.push_back(vtcRaw[(idx * 3) + a]);
		}
	}

	std::vector<GLfloat> st;
	for (int i = 0; i < idcSt.size(); i++) {
		int idx = idcSt[i];

		for (int a = 0; a < 2; a++) {
			st.push_back(stRaw[(idx * 2) + a]);
		}
	}

	std::vector<GLushort> idc;
	for (int i = 0; i < idcVtc.size(); i++) {
		idc.push_back(i);
	}

	Obj* inst = objMk(&vtc[0], &st[0], &idc[0], idc.size(), Mesh::OBJ, vtx, frag, tex, active, loc, rot);

	return inst;
}

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, unsigned int type, std::string vtx, std::string frag, std::string tex, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	Obj* inst = objMk(vtc, idc, noPrim, type, vtx, frag, active, child, noChild, loc, rot);

	glBindVertexArray(inst->_mesh->_id[Mesh::VAO]);

	glGenBuffers(1, &inst->_mesh->_id[Mesh::STBO]);
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::STBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 2 * sizeof (GLfloat), st, GL_STATIC_DRAW);

	inst->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::STBO]);
	inst->_attr[Obj::ST] = glGetAttribLocation(inst->_prog._id, "st");
	glVertexAttribPointer(inst->_attr[Obj::ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(inst->_attr[Obj::ST]);

	return inst;
}

Obj* objMk(std::string name, std::string vtx, std::string frag, std::string tex, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	std::vector<GLfloat> vtcRaw = util::mesh::rd::attr(name, Obj::POS);
	std::vector<GLushort> idcVtc = util::mesh::rd::idc(name, Obj::POS);

	std::vector<GLfloat> stRaw = util::mesh::rd::attr(name, Obj::ST);
	std::vector<GLushort> idcSt = util::mesh::rd::idc(name, Obj::ST);

	std::vector<GLfloat> vtc;
	for (int i = 0; i < idcVtc.size(); i++) {
		int idx = idcVtc[i];

		for (int a = 0; a < 3; a++) {
			vtc.push_back(vtcRaw[(idx * 3) + a]);
		}
	}

	std::vector<GLfloat> st;
	for (int i = 0; i < idcSt.size(); i++) {
		int idx = idcSt[i];

		for (int a = 0; a < 2; a++) {
			st.push_back(stRaw[(idx * 2) + a]);
		}
	}

	std::vector<GLushort> idc;
	for (int i = 0; i < idcVtc.size(); i++) {
		idc.push_back(i);
	}

	Obj* inst = objMk(&vtc[0], &st[0], &idc[0], idc.size(), Mesh::OBJ, vtx, frag, tex, active, child, noChild, loc, rot);

	return inst;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int type, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* inst = (Obj*) malloc(sizeof (Obj));

	inst->_noUni = 5;
	inst->_uni = (GLint*) malloc(inst->_noUni * sizeof (GLint));
	inst->_active = active;
	inst->_type = type;
	inst->_child = nullptr;
	inst->_noChild = 0;
	inst->_v = 0.0;
	inst->_tex = 0;

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			inst->_aabb[a][b] = 0.0;
		}
	}

	inst->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	inst->_model = glm::mat4(1.0);
	inst->_model *= glm::translate(glm::mat4(1.0), loc);
	inst->_model *= util::matr::rot(glm::mat4(1.0), rot);
	inst->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	inst->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
	inst->_acc = glm::mat4(1.0);

	inst->_prog = Prog(vtx, geom, frag);

	inst->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::VBO]);
	inst->_attr[Obj::POS] = glGetAttribLocation(inst->_prog._id, "pos");
	glVertexAttribPointer(inst->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(inst->_attr[Obj::POS]);

	// uniform
	inst->_uni[Obj::MODEL] = glGetUniformLocation(inst->_prog._id, "model");
	inst->_uni[Obj::VIEW] = glGetUniformLocation(inst->_prog._id, "view");
	inst->_uni[Obj::PROJ] = glGetUniformLocation(inst->_prog._id, "proj");

	inst->_uni[Obj::ACTIVE] = glGetUniformLocation(inst->_prog._id, "active");

	inst->_uni[Obj::T] = glGetUniformLocation(inst->_prog._id, "t");

	glUniformMatrix4fv(inst->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(inst->_model));
	glUniformMatrix4fv(inst->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(inst->_view));
	glUniformMatrix4fv(inst->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(inst->_proj));

	glUniform1ui(inst->_uni[Obj::ACTIVE], inst->_active);

	glUniform1ui(inst->_uni[Obj::T], disp->t);

	inst->_prog.unUse();
	glBindVertexArray(0);

	// rig
	objAcc(inst, glm::mat4(1.0));

	return inst;
}

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int type, std::string vtx, std::string geom, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot) {
	// initialize
	Obj* inst = (Obj*) malloc(sizeof (Obj));

	inst->_noUni = 5;
	inst->_uni = (GLint*) malloc(inst->_noUni * sizeof (GLint));
	inst->_active = active;
	inst->_type = type;
	inst->_v = 0.0;
	inst->_tex = 0;
	inst->_noChild = noChild;
	inst->_child = (Obj**) malloc(inst->_noChild * sizeof (Obj*));
	for (int i = 0; i < inst->_noChild; i++) {
		inst->_child[i] = child[i];
	}

	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			inst->_aabb[a][b] = 0.0;
		}
	}

	inst->_mesh = meshMk(vtc, idc, noPrim);

	// matrix
	inst->_model = glm::mat4(1.0);
	inst->_model *= glm::translate(glm::mat4(1.0), loc);
	inst->_model *= util::matr::rot(glm::mat4(1.0), rot);
	inst->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	inst->_proj = glm::ortho(-(layout::view[X] / 2.0), layout::view[X] / 2.0, -(layout::view[Y] / 2.0), layout::view[Y] / 2.0, 0.1, 100000.0);
	inst->_acc = glm::mat4(1.0);

	inst->_prog = Prog(vtx, geom, frag);

	inst->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, inst->_mesh->_id[Mesh::VBO]);
	inst->_attr[Obj::POS] = glGetAttribLocation(inst->_prog._id, "pos");
	glVertexAttribPointer(inst->_attr[Obj::POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(inst->_attr[Obj::POS]);

	// uniform
	inst->_uni[Obj::MODEL] = glGetUniformLocation(inst->_prog._id, "model");
	inst->_uni[Obj::VIEW] = glGetUniformLocation(inst->_prog._id, "view");
	inst->_uni[Obj::PROJ] = glGetUniformLocation(inst->_prog._id, "proj");

	inst->_uni[Obj::ACTIVE] = glGetUniformLocation(inst->_prog._id, "active");

	inst->_uni[Obj::T] = glGetUniformLocation(inst->_prog._id, "t");

	glUniformMatrix4fv(inst->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(inst->_model));
	glUniformMatrix4fv(inst->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(inst->_view));
	glUniformMatrix4fv(inst->_uni[Obj::PROJ], 1, GL_FALSE, glm::value_ptr(inst->_proj));

	glUniform1ui(inst->_uni[Obj::ACTIVE], inst->_active);

	glUniform1ui(inst->_uni[Obj::T], disp->t);

	inst->_prog.unUse();
	glBindVertexArray(0);

	// rig
	objAcc(inst, glm::mat4(1.0));

	return inst;
}

void objDel(Obj* inst) {
	meshDel(inst->_mesh);

	free(inst->_uni);

	glDeleteTextures(1, &inst->_tex);

	for (int i = 0; i < inst->_noChild; i++) {
		if (inst->_child[i]) {
			objDel(inst->_child[i]);
		}
	}
	free(inst->_child);

	free(inst);
}

void objAcc(Obj* inst, glm::mat4 prev) {
	inst->_acc = prev * inst->_model;

	for (int i = 0; i < inst->_noChild; i++) {
		if (inst->_child[i]) {
			objAcc(inst->_child[i], inst->_acc);
		}
	}
}

void objAnim(Obj* inst, Obj* parent, glm::vec3 loc, glm::vec3 rot, GLfloat speed) {
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

			inst->_model *= trans;

			glm::mat4 prev;
			if (parent) {
				prev = parent->_acc;
			} else {
				prev = glm::mat4(1.0);
			}

			objAcc(inst, prev);

			locFrame += glm::abs(stepLoc);
			rotFrame += glm::abs(stepRot);

			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));
		}
	} else {
		glm::mat4 trans = glm::mat4(1.0);
		trans = glm::translate(trans, loc);
		trans = util::matr::rot(trans, rot);

		objAcc(inst, inst->_acc * trans);
	}
}

void objMv(Obj* inst, Obj* parent, glm::vec3 loc, glm::vec3 rot, GLfloat speed) {
	bool coll = false;
	glm::mat4 dest = glm::mat4(1.0);
	dest = glm::translate(dest, loc);
	dest = util::matr::rot(dest, rot);

	for (int i = 0; i < scn::obj.size(); i++) {
		if (inst != scn::obj[i]) {
			if (util::phys::aabb(inst, scn::obj[i])) {
				coll = true;

				break;
			}
		}
	}

	objAnim(inst, parent, loc, rot, speed);

/* 	if (!coll) { */
/* 		objAnim(inst, parent, loc, rot, speed); */
/* 	} else { */
/* 		omni::err(omni::ERR_OBJ_CLIP); */
/* 	} */
}

void objA(Obj* inst) {
	int i = 0;
	while (!util::phys::aabbGround(inst)) {
		objMv(inst, nullptr, glm::vec3(0.0, -(pow(i, 2) * phys::g), 0.0), glm::vec3(0.0));

		i++;
	}
}

void objDraw(Obj* inst) {
	GLenum prim;
	switch (inst->_type) {
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

	inst->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(-10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	inst->_view = glm::scale(inst->_view, cam._scale);

	glBindVertexArray(inst->_mesh->_id[Mesh::VAO]);
	inst->_prog.use();
	glBindTexture(GL_TEXTURE_2D, inst->_tex);

	glUniformMatrix4fv(inst->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(inst->_acc));
	glUniformMatrix4fv(inst->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(inst->_view));

	glUniform1ui(inst->_uni[Obj::T], disp->t);

	glUniform1ui(inst->_uni[Obj::ACTIVE], inst->_active);

	glDrawElements(prim, inst->_mesh->noPrim, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	inst->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < inst->_noChild; i++) {
		if (inst->_child[i]) {
			objDraw(inst->_child[i]);
		}
	}
}
