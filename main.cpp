#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <Python.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glm/gtc/type_ptr.hpp>

#include "scn.h"
#include "util.h"
#include "state.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "omni.h"
#include "col.h"
#include "layout.h"
#include "class_def.h"

Disp* disp;
Console* console;

Cam cam;

bool boot = false;

extern std::string stage;
extern unsigned int lvl;

int main(int argc, char* argv[]) {
	disp = new Disp("Omni", {
		500,
		500
	}, col[false]);

	GLfloat vtc[] = {
		-0.26, 0.0, -0.26,
		0.16, 0.0, -0.26,
		-0.10, 0.0, -0.10,
		0.06, 0.0, -0.10,

		0.16, 0.0, -0.26,
		0.26, 0.0, -0.16,
		0.06, 0.0, -0.10,
		0.10, 0.0, -0.06,

		0.26, 0.0, -0.16,
		0.26, 0.0, 0.26,
		0.10, 0.0, -0.06,
		0.10, 0.0, 0.10,

		0.26, 0.0, 0.26,
		-0.16, 0.0, 0.26,
		0.10, 0.0, 0.10,
		-0.06, 0.0, 0.10,

		-0.16, 0.0, 0.26,
		-0.26, 0.0, 0.16,
		-0.06, 0.0, 0.10,
		-0.10, 0.0, 0.06,

		-0.10, 0.0, 0.06,
		-0.26, 0.0, 0.16,
		-0.10, 0.0, -0.10,
		-0.26, 0.0, -0.26
	};

	GLushort idc[] = {
		0, 1, 2,
		2, 1, 3,

		4, 5, 6,
		6, 5, 7,

		8, 9, 10,
		10, 9, 11,

		12, 13, 14,
		14, 13, 15,

		16, 17, 18,
		18, 17, 19,

		20, 21, 22,
		22, 21, 23
	};

	Mesh* mesh = meshMk(vtc, idc, sizeof idc / sizeof *idc);

	Prog prog("logo", "solid");

	prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, mesh->_id[Mesh::VBO]);
	GLint attrPos = glGetAttribLocation(prog._id, "pos");
	glVertexAttribPointer(attrPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(attrPos);

	// uniform
	GLint uniRes = glGetUniformLocation(prog._id, "res");
	GLint uniActive = glGetUniformLocation(prog._id, "active");

	glUniform2ui(uniRes, disp->_res[X], disp->_res[Y]);

	glUniform1ui(uniActive, true);

	prog.unUse();

	// draw
	disp->clear();

	glBindVertexArray(mesh->_id[Mesh::VAO]);
	prog.use();

	glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	prog.unUse();
	glBindVertexArray(0);

	meshDel(mesh);

	disp->update();

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	return 0;
}
