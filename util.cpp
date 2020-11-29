#include <iostream>
#include <sstream>

#include "util.h"

template <typename T>
T util::fs::rd(std::string name) {
}

template <>
std::string util::fs::rd<std::string>(std::string name) {
	std::ifstream in;
	in.open(name);

	std::string cont;
	for (std::string l; std::getline(in, l);) {
		cont += l + "\n";
	}

	in.close();

	return cont;
}

template <>
std::vector<std::string> util::fs::rd<std::vector<std::string>>(std::string name) {
	std::ifstream in;
	in.open("./" + name);

	std::vector<std::string> cont;
	for (std::string l; std::getline(in, l);) {
		cont.push_back(l);
	}

	in.close();

	return cont;
}

std::vector<std::string> util::str::split(std::string buff, char delim) {
	std::vector<std::string> tok;

	std::stringstream s(buff);
	std::string seg;
	while (std::getline(s, seg, delim)) {
		tok.push_back(seg);
	}

	return tok;
}

std::vector<GLfloat> util::mesh::rd::vtc(std::string name) {
	std::vector<GLfloat> obj;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "v") {
			for (int i = 1; i < 1 + 3; i++) {
				obj.push_back(std::stof(tok[i]));
			}
		}
	}

	return obj;
}

std::vector<GLushort> util::mesh::rd::idc(std::string name) {
	std::vector<GLushort> obj;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "f") {
			for (int i = 1; i < 1 + 3; i++) {
				obj.push_back(std::stoi(tok[i]) - 1);
			}
		}
	}

	return obj;
}
