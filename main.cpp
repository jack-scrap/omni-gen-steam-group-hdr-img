#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>

#include "hjkl.h"
#include "util.h"

bool run = true;

std::string buff = rd("asdf.py");

int main() {
	Py_Initialize();

	void* handle = dlopen("./libasdf.so", RTLD_LAZY);

	void (*asdfDraw)(Asdf* asdf) = (void (*)(Asdf*)) dlsym(handle, "asdfDraw");

	SDL_Event e;
	while (run) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_F1) {
					PyRun_SimpleString(buff.c_str());
				}
			}

			if (e.type == SDL_QUIT) {
				run = false;
			}
		}

		disp.clear(0.16, 0.16, 0.16);

		asdfDraw(&asdf);

		disp.update();
	}

	return 0;
}
