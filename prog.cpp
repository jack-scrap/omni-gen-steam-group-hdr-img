#include <iostream>
#include <GL/glew.h>

#include "prog.h"
#include "util.h"

Shad::Shad(int stage) {
	std::string ext;
	GLenum type;
	if (stage) {
		ext = "fs";
		type = GL_FRAGMENT_SHADER;
	} else {
		ext = "vs";
		type = GL_VERTEX_SHADER;
	}

	std::string txt = rd("shad." + ext);
	const char* src = txt.c_str();

	GLint succ;
	char buff[] = "";

	_id = glCreateShader(type);
	glShaderSource(_id, 1, &src, NULL);
	glCompileShader(_id);

	glGetShaderiv(_id, GL_COMPILE_STATUS, &succ);
	if (!succ) {
		glGetShaderInfoLog(_id, 512, NULL, buff);
		std::cout << "Error: " << std::endl;
		std::cout << buff << std::endl;
	}
}

Prog::Prog() {
	// shader
	Shad
		vtx(0),
		frag(1);

	// program
	id = glCreateProgram();
	glAttachShader(id, vtx._id);
	glAttachShader(id, frag._id);
	glLinkProgram(id);
}

void Prog::use() {
	glUseProgram(id);
}

void Prog::unUse() {
	glUseProgram(0);
}
