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

std::string buff = util::fs::rd<std::string>("asdf.py");
std::vector<std::string> buffVec = util::fs::rd<std::vector<std::string>>("asdf.py");

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
	Console console(buffVec, ln, {
		disp._wd, disp._ht
	});

	std::thread input(kb);
	input.detach();

	void* handle = dlopen("libpoly.so", RTLD_LAZY);

	void (*polyDraw)(Poly* poly) = (void (*)(Poly*)) dlsym(handle, "polyDraw");

	while (run) {
		disp.clear(0.16, 0.16, 0.16);

		console.print(buffVec);

		polyDraw(&tri);

		disp.update();
	}

	return 0;
}
