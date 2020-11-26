#include <iostream>
#include <Python.h>
#include <dlfcn.h>

#include "hjkl.h"
#include "util.h"

int main() {
	Py_Initialize();

	std::string buff = rd("asdf.py");

	PyRun_SimpleString(buff.c_str());

	void* handle = dlopen("./libasdf.so", RTLD_LAZY);

	void (*asdfDraw)(Asdf* asdf) = (void (*)(Asdf*)) dlsym(handle, "asdfDraw");

	std::cout << dlerror() << std::endl;

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
