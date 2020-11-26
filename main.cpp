#include <iostream>
#include <Python.h>

#include "asdf.h"
#include "hjkl.h"
#include "util.h"

int main() {
	Py_Initialize();

	std::string buff = rd("asdf.py");

	/* PyRun_SimpleString(buff.c_str()); */

	SDL_Event e;
	while (true) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				SDL_Quit();
			}
		}

		disp.clear(0.16, 0.16, 0.16);

		asdfDraw(&asdf);

		disp.update();
	}

	return 0;
}
