#include <iostream>
#include <Python.h>

#include "asdf.h"
#include "hjkl.h"
#include "util.h"

int main() {
	Py_Initialize();

	std::string buff = rd("asdf.py");

	std::cout << asdf._asdf << std::endl;

	PyRun_SimpleString(buff.c_str());

	std::cout << asdf._asdf << std::endl;

	return 0;
}
