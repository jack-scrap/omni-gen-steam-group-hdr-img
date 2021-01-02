#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>

#include "scn.h"
#include "util.h"
#include "state.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"

bool
	run,
	drag;

glm::vec3 prev;

int
	begin[2],
	curr[2],
	delta[2];

Console* console;

int main() {
	console = new Console(util::fs::rd<std::vector<std::string>>("script/0.py"));

	SDL_Event e;
	while (disp._open) {
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
							console->enter();

							break;

						case SDLK_BACKSPACE:
							console->pop();

							break;

						case SDLK_F1:
							console->_mode = Console::EDITOR;

							console->render();

							break;

						case SDLK_F2:
							console->_mode = Console::CMD;

							console->render();

							break;
					}	
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				drag = true;

				SDL_GetMouseState(&begin[0], &begin[1]);

				prev = cam._pos;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				drag = false;
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (drag) {
					SDL_GetMouseState(&curr[0], &curr[1]);

					delta[0] = begin[0] - curr[0];
					delta[1] = begin[1] - curr[1];

					cam._pos[0] = prev.x + delta[0];
					cam._pos[2] = prev.y + delta[1];
				}
			}

			if (e.type == SDL_MOUSEWHEEL) {
				if (e.wheel.y > 0) {
					if (glm::all(glm::lessThan(cam._scale, glm::vec3(100.0, 100.0, 100.0)))) {
						cam._scale += glm::vec3(10.0, 10.0, 10.0);
					}
				}

				if (e.wheel.y < 0) {
					if (glm::all(glm::greaterThan(cam._scale, glm::vec3(20.0, 20.0, 20.0)))) {
						cam._scale -= glm::vec3(10.0, 10.0, 10.0);
					}
				}
			}

			if (e.type == SDL_QUIT) {
				disp._open = false;
			}
		}

		disp.draw();
	}

	return 0;
}
