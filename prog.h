#pragma once

#include <string>

#include <GL/glew.h>
#include <fstream>

class Shad {
	public:
		GLuint _id;

		Shad(std::string name, unsigned int stage);
};

class Prog {
	public:
		GLuint id;

		Prog(std::string nameVtx, std::string nameFrag);

		void use();

		void unUse();
};
