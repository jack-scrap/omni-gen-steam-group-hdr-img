#include <iostream>
#include <Python.h>
#include <string>
#include <fstream>

#include "asdf.h"
#include "hjkl.h"

std::string rd(std::string name) {
	std::ifstream in;
	in.open(name);

	std::string cont;
	for (std::string l; std::getline(in, l);) {
		cont += l + "\n";

	}

	in.close();

	return cont;
}

int main() {
	Py_Initialize();

	std::string init = rd("asdf.py");

	std::cout << asdf._asdf << std::endl;

	PyRun_SimpleString(init.c_str());

	std::cout << asdf._asdf << std::endl;

	return 0;
}
