#include <iostream>
#include <Python.h>

#include "asdf.h"
#include "hjkl.h"

int main() {
	Py_Initialize();

	while (true) {
		std::cout << asdf._asdf << std::endl;
	}

	return 0;
}
