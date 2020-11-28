#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>

#include "scn.h"
#include "console.h"
#include "util.h"

unsigned int ln = 36;

bool run = true;

std::string buff = util::rd("asdf.py");
std::vector<std::string> buffVec = util::rdVec("asdf.py");

SDL_Event e;
void kb() {
	while (true) {
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
	void* handle = dlopen("libpoly.so", RTLD_LAZY);

	void (*polyDraw)(Poly* poly) = (void (*)(Poly*)) dlsym(handle, "polyDraw");

	Py_Initialize();

	std::thread input(kb);
	input.detach();

	TTF_Init();

	TTF_Font* font = TTF_OpenFont("terminus.bdf", 24);

	Console console(buffVec, ln, {
		800, 600
	});

	while (run) {
		disp.clear(0.16, 0.16, 0.16);

		console.print(buffVec);

		polyDraw(&tri);

		disp.update();
	}

	return 0;
}
