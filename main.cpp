#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>

#include "hjkl.h"
#include "util.h"

bool run = true;

std::string buff = rd("asdf.py");

int main() {
	void* handle = dlopen("./libpoly.so", RTLD_LAZY);

	void (*polyDraw)(Poly* poly) = (void (*)(Poly*)) dlsym(handle, "polyDraw");

	Py_Initialize();

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

		polyDraw(&tri);

		disp.update();
	}

	return 0;
}
