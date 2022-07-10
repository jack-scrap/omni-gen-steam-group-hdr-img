#pragma once

#include <string>

#include <GL/glew.h>
#include <fstream>

class Shad {
	public:
		GLuint id;

		Shad(std::string name, unsigned int stage);
};

class Prog {
	public:
		GLuint _id;

		Prog(std::string nameVtx, std::string nameFrag);

		Prog(std::string nameVtx, std::string nameGeom, std::string nameFrag);

		void use();

		void unUse();
};
