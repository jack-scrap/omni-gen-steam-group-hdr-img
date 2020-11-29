#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>

#include "scn.h"
#include "console.h"
#include "util.h"
#include "state.h"

bool run = true;

std::string buff = util::fs::rd<std::string>("asdf.py");
std::vector<std::string> buffVec = util::fs::rd<std::vector<std::string>>("asdf.py");

glm::vec2 scr = glm::vec2(800, 600);

Console* console;

int main() {
	Console* console = new Console(buffVec, state::ln, disp._res);

	void* handle = dlopen("libpoly.so", RTLD_LAZY);

	void (*polyDraw)(Poly* poly) = (void (*)(Poly*)) dlsym(handle, "polyDraw");

	TTF_Font* font = TTF_OpenFont("res/terminus.bdf", 24);

	SDL_Event e;
	while (run) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
					console->_bg.back().push_back(Bg(false, {
						console->_bg.back().size(), console->_bg.size() - 1
					}));
					console->_buff.back().push_back(Char((char) e.key.keysym.sym, false, font, {
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

						case SDLK_RETURN:
							console->_bg.push_back({});
							console->_buff.push_back({});

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

		disp.clear();

		glEnable(GL_DEPTH_TEST);

		glViewport(console->_res[0], 0, console->_res[0] + scr[0], scr[1]);

		polyDraw(&tri);

		glViewport(0, 0, console->_res[0], scr[1]);

		console->print();

		disp.update();
	}

	return 0;
}
