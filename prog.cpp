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

	_id = glCreateShader(type);
	glShaderSource(_id, 1, &src, NULL);
	glCompileShader(_id);

	GLint succ;
	char err[] = "";

	glGetShaderiv(_id, GL_COMPILE_STATUS, &succ);
	if (!succ) {
		glGetShaderInfoLog(_id, 512, NULL, err);
		omni::err("Shader compilation failed...");
		omni::err(std::string(err));
	}
}

Prog::Prog(std::string nameVtx, std::string nameFrag) {
	// shader
	Shad
		vtx(nameVtx, 0),
		frag(nameFrag, 1);

	// program
	_id = glCreateProgram();
	glAttachShader(_id, vtx._id);
	glAttachShader(_id, frag._id);
	glLinkProgram(_id);
}

Prog::Prog(std::string nameVtx, std::string nameGeom, std::string nameFrag) {
	// shader
	Shad
		vtx(nameVtx, 0),
		geom(nameGeom, 2),
		frag(nameFrag, 1);

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
