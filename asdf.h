#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"

#pragma once

#include "util.h"

class Shad {
	public:
		GLuint _id;

		Shad(int stage) {
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
};

class Prog {
	public:
		GLuint id;

		Prog() {
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

		void use() {
			glUseProgram(id);
		}

		void unUse() {
			glUseProgram(0);
		}
};

typedef struct {
	int _asdf;

	GLuint
		_vao,
		_vbo;

	Prog _prog;
} Asdf;

Asdf asdfMk();

extern "C" void asdfDraw(Asdf* asdf);

extern "C" void set(Asdf* asdf, int hjkl);
