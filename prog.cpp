#include <iostream>
#include <string>
#include <GL/glew.h>

#include "prog.h"
#include "util.h"

Shad::Shad(std::string name, unsigned int stage) {
	std::string ext;
	GLenum type;
	switch (stage) {
		case 0:
			ext = "vs";
			type = GL_VERTEX_SHADER;

			break;

		case 1:
			ext = "fs";
			type = GL_FRAGMENT_SHADER;

			break;

		case 2:
			ext = "gs";
			type = GL_GEOMETRY_SHADER;

			break;
	}

	std::string buff = util::fs::rd<std::string>("res/shad/" + name + "." + ext);
	const char* src = buff.c_str();

	GLint succ;
	char err[] = "";

	_id = glCreateShader(type);
	glShaderSource(_id, 1, &src, NULL);
	glCompileShader(_id);

	glGetShaderiv(_id, GL_COMPILE_STATUS, &succ);
	if (!succ) {
		glGetShaderInfoLog(_id, 512, NULL, err);
		std::cout << "Error: " << std::endl;
		std::cout << err << std::endl;
	}
}

Prog::Prog(std::string nameVtx, std::string nameFrag) {
	// shader
	Shad
		vtx(nameVtx, 0),
		frag(nameFrag, 1);

	// program
	id = glCreateProgram();
	glAttachShader(id, vtx._id);
	glAttachShader(id, frag._id);
	glLinkProgram(id);
}

Prog::Prog(std::string nameVtx, std::string nameGeom, std::string nameFrag) {
	// shader
	Shad
		vtx(nameVtx, 0),
		geom(nameGeom, 2),
		frag(nameFrag, 1);

	// program
	id = glCreateProgram();
	glAttachShader(id, vtx._id);
	glAttachShader(id, geom._id);
	glAttachShader(id, frag._id);
	glLinkProgram(id);
}

void Prog::use() {
	glUseProgram(id);
}

void Prog::unUse() {
	glUseProgram(0);
}
