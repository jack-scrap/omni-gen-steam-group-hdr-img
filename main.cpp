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
#include "obj.h"

bool run = true;

std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("asdf.py");

glm::vec2 scr = glm::vec2(800, 600);

Console* console;

int main() {
	Console* console = new Console(buff, state::ln, disp._res);

	void* handle = dlopen("libpoly.so", RTLD_LAZY);

	void (*polyDraw)(Poly* poly) = (void (*)(Poly*)) dlsym(handle, "polyDraw");

	TTF_Font* font = TTF_OpenFont("res/terminus.bdf", 24);

	SDL_Event e;
	while (run) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
					console->_buff.back().push_back((char) e.key.keysym.sym);

					console->_bg.back().push_back(Bg(false, {
						console->_bg.back().size(), console->_bg.size() - 1
					}));
					console->_txt.back().push_back(Char((char) e.key.keysym.sym, true, font, {
						console->_txt.back().size(), console->_bg.size() - 1
					}));
				} else {
					switch (e.key.keysym.sym) {
						case SDLK_SPACE:
							console->_buff.back().push_back((char) e.key.keysym.sym);

							console->_bg.back().push_back(Bg(false, {
								console->_bg.back().size(), console->_bg.size() - 1
							}));
							console->_txt.back().push_back(Char((char) e.key.keysym.sym, true, font, {
								console->_txt.back().size(), console->_bg.size() - 1
							}));

						break;

						case SDLK_BACKSPACE:
							if (!console->_buff.empty()) {
								if (!console->_buff.back().empty() &&!console->_txt.back().empty()) {
									console->_buff.back().pop_back();

									console->_bg.back().pop_back();
									console->_txt.back().pop_back();
								} else {
									console->_buff.pop_back();

									console->_bg.pop_back();
									console->_txt.pop_back();
								}
							}

							break;

						case SDLK_RETURN:
							console->_buff.push_back("");

							console->_bg.push_back({});
							console->_txt.push_back({});

							break;

						case SDLK_F1:
							std::string str;
							for (const std::string& l : console->_buff) {
								str += l + '\n';
							}

							PyRun_SimpleString(str.c_str());

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

		glViewport(console->_res[0], 0, scr[0], scr[1]);

		objDraw(&front);

		asdfDraw(&asdf);

		glViewport(0, 0, console->_res[0], scr[1]);

		console->print();

		disp.update();
	}

	return 0;
}
