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

Console* console;

int main() {
	Console* console = new Console(buffVec, ln, {
		disp._res[0], disp._res[1]
	});

	void* handle = dlopen("libpoly.so", RTLD_LAZY);

	void (*polyDraw)(Poly* poly) = (void (*)(Poly*)) dlsym(handle, "polyDraw");

	TTF_Font* font = TTF_OpenFont("res/terminus.bdf", 24);

	SDL_Event e;
	while (run) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
					console->_bg.back().push_back(Bg(true, {
						console->_bg.back().size(), console->_bg.size() - 1
					}));
					console->_buff.back().push_back(Char((char) e.key.keysym.sym, true, font, {
						console->_buff.back().size(), console->_bg.size() - 1
					}));
				} else {
					switch (e.key.keysym.sym) {
						case SDLK_BACKSPACE:
							if (!console->_bg.empty() && !console->_buff.empty()) {
								if (!console->_bg.back().empty() &&!console->_buff.back().empty()) {
									console->_bg.back().pop_back();
									console->_buff.back().pop_back();
								} else {
									console->_bg.pop_back();
									console->_buff.pop_back();
								}
							}

							break;

						case SDLK_F1:
							PyRun_SimpleString(buff.c_str());

							break;
					}
				}
			}

			if (e.type == SDL_QUIT) {
				run = false;
			}
		}

		disp.clear(col[false]);

		glEnable(GL_DEPTH_TEST);

		glViewport(console->_res[0], 0, disp._res[0] - console->_res[0], disp._res[1]);

		polyDraw(&tri);

		glViewport(0, 0, console->_res[0], disp._res[1]);

		console->print();

		disp.update();
	}

	return 0;
}
