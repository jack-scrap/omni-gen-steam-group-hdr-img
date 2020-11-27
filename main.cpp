#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>

#include "hjkl.h"
#include "util.h"

bool run = true;

std::string buff = rd("asdf.py");

SDL_Event e;
void kb() {
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
	}
}

int main() {
	Py_Initialize();

	void* handle = dlopen("./libasdf.so", RTLD_LAZY);

	void (*asdfDraw)(Asdf* asdf) = (void (*)(Asdf*)) dlsym(handle, "asdfDraw");

	std::thread input(kb);
	input.detach();

	while (run) {
		disp.clear(0.16, 0.16, 0.16);

		asdfDraw(&asdf);

		disp.update();
	}

	return 0;
}
