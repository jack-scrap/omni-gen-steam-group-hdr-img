#include <iostream>
#include <string>
#include <GL/glew.h>

#include "prog.h"
#include "util.h"
#include "omni.h"

Shad::Shad(std::string name, unsigned int stage) {
	std::string ext;
	GLenum type;
	switch (stage) {
		case 0:
			type = GL_VERTEX_SHADER;
			ext = "vs";

			break;

		case 1:
			type = GL_FRAGMENT_SHADER;
			ext = "fs";

			break;

		case 2:
			type = GL_GEOMETRY_SHADER;
			ext = "gs";

			break;
	}

	std::string buff = util::fs::rd<std::string>(util::fs::path::build({
		"res",
		"shad",
		name + "." + ext
	}));
	const char* src = buff.c_str();

	_id = glCreateShader(type);
	glShaderSource(_id, 1, &src, NULL);
	glCompileShader(_id);

	GLint succ;
	char err[] = "";

	glGetShaderiv(_id, GL_COMPILE_STATUS, &succ);
	if (!succ) {
		glGetShaderInfoLog(_id, 512, NULL, err);
		omni::err(omni::ERR_SHADER);
		std::cerr << err << std::endl;
	}
}

Prog::Prog(std::string nameVtx, std::string nameFrag) {
	// stage
	Shad vtx(nameVtx, 0);
	Shad frag(nameFrag, 1);

	// program
	_id = glCreateProgram();
	glAttachShader(_id, vtx._id);
	glAttachShader(_id, frag._id);
	glLinkProgram(_id);
}

Prog::Prog(std::string nameVtx, std::string nameGeom, std::string nameFrag) {
	// stage
	Shad vtx(nameVtx, 0);
	Shad geom(nameGeom, 2);
	Shad frag(nameFrag, 1);

	// program
	_id = glCreateProgram();
	glAttachShader(_id, vtx._id);
	glAttachShader(_id, geom._id);
	glAttachShader(_id, frag._id);
	glLinkProgram(_id);
}

void Prog::use() {
	glUseProgram(_id);
}

void Prog::unUse() {
	glUseProgram(0);
}
