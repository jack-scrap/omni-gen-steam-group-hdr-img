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

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;
	_->_active = active;
	_->_noChild = 0;
	_->_loc = loc;

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
	_->_view = glm::lookAt(glm::vec3(1000.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, glm::vec3(50));
	_->_model = glm::translate(_->_model, _->_loc);

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

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc) {
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
	_->_view = glm::lookAt(glm::vec3(1000.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, glm::vec3(50));
	_->_model = glm::translate(_->_model, _->_loc);

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

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	_->_prog.unUse();

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;
	_->_active = active;
	_->_noChild = 0;
	_->_loc = loc;

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
	_->_view = glm::lookAt(glm::vec3(1000.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, glm::vec3(50));
	_->_model = glm::translate(_->_model, _->_loc);

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

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	return _;
}

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc) {
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
	_->_view = glm::lookAt(glm::vec3(1000.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, glm::vec3(50));
	_->_model = glm::translate(_->_model, _->_loc);

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

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	return _;
}

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_noChild = 0;
	_->_loc = loc;

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
	_->_view = glm::lookAt(glm::vec3(1000.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::scale(_->_model, glm::vec3(50));
	_->_model = glm::translate(_->_model, _->_loc);

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

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	return _;
}

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_active = active;
	_->_child = (Obj**) malloc(noChild * sizeof (Obj*));
	for (int i = 0; i < noChild; i++) {
		_->_child[i] = child[i];
	}
	_->_noChild = noChild;
	_->_loc = loc;

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
	_->_view = glm::lookAt(glm::vec3(1000.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	_->_model = glm::mat4(1.0);
	_->_model = glm::translate(_->_model, _->_loc);
	_->_model = glm::scale(_->_model, glm::vec3(50));

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

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	return _;
}

void objMv(Obj* obj, GLfloat* d) {
	for (int i = 0; i < 3; i++) {
		obj->_loc[i] += d[i];
	}

	obj->_model = glm::scale(obj->_model, glm::vec3(50));
	obj->_model = glm::translate(obj->_model, glm::vec3(d[X], d[Y], d[Z]));

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objMv(obj->_child[i], d);
		}
	}
}

Obj* objGet() {
	return wheel;
}

void objSet(Obj* obj, GLfloat* d) {
	unsigned int fps = 10;

	float step[3];
	for (int i = 0; i < 3; i++) {
		step[i] = d[i] / fps;
	}

	for (int t = 0; t < fps; t++) {
		for (int i = 0; i < 3; i++) {
			obj->_loc[i] += step[i];
		}

		obj->_model = glm::translate(glm::mat4(1.0), obj->_loc);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
	}
}

void objDraw(Obj* obj) {
	glBindVertexArray(obj->_id[VAO]);

	obj->_prog.use();

	obj->_model = glm::mat4(1.0);
	obj->_model = glm::scale(obj->_model, glm::vec3(50));
	obj->_model = glm::translate(obj->_model, obj->_loc);

	glUniformMatrix4fv(obj->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(obj->_model));
	glUniformMatrix4fv(obj->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));
	glUniformMatrix4fv(obj->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(obj->_proj));

	glUniform1ui(obj->_uni[ACTIVE], obj->_active);

	glDrawElements(GL_TRIANGLES, obj->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < obj->_noChild; i++) {
		if (&obj->_child[i]) {
			objDraw(obj->_child[i]);
		}
	}
}
