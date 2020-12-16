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
#include "truck.h"
#include "pt.h"

bool run = true;

std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("asdf.py");

glm::vec2 scr = glm::vec2(800, 600);

Console* console;

int main() {
	Console* console = new Console(buff);

	TTF_Font* font = TTF_OpenFont("res/terminus.bdf", 24);

	SDL_Event e;
	while (run) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
					console->push((char) e.key.keysym.sym);
				} else {
					switch (e.key.keysym.sym) {
						case SDLK_SPACE:
							console->push(' ');

							break;

						case SDLK_RETURN:
							console->newline();

							break;

						case SDLK_BACKSPACE:
							console->pop();

							break;

						case SDLK_F1:
							console->_mode = EDITOR;

							break;

						case SDLK_F2:
							console->_mode = CMD;

							break;

						case SDLK_F3:
							PyRun_SimpleString(util::str::join(console->_buff).c_str());

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

		glViewport(state::res[0], 0, scr[0], scr[1]);

		truckDraw(truck);

		glViewport(0, 0, state::res[1], scr[1]);

		console->draw();

		disp.update();
	}

	return 0;
}
