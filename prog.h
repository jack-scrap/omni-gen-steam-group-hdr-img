#pragma once

#include <string>

#include <GL/glew.h>
#include <fstream>

std::string rd(std::string name);

class Shad {
	public:
		GLuint _id;

		Shad(int stage);
};

class Prog {
	public:
		GLuint id;

		Prog();

		void use();

		void unUse();
};
