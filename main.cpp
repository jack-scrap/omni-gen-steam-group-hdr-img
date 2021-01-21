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

bool run;

Console* console;

int main() {
	console = new Console(util::fs::rd<std::vector<std::string>>("script/0.py"));

	GLfloat d[3] = {
		0.3, 0.0, 0.0
	};

	SDL_Event e;
	while (disp._open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				// modifier
				if (e.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
					if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
						console->push((char) (e.key.keysym.sym - 32));
					}
				} else {
					if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
						console->push((char) e.key.keysym.sym);
					} else if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
						console->push((char) e.key.keysym.sym);
					} else {
						switch (e.key.keysym.sym) {
							case SDLK_SPACE:
								console->push(' ');

								break;

							case SDLK_BACKQUOTE:
								console->push('`');

								break;

							case SDLK_MINUS:
								console->push('-');

								break;

							case SDLK_EQUALS:
								console->push('=');

								break;

								// bracket
							case SDLK_LEFTBRACKET:
								console->push('[');

								break;

							case SDLK_RIGHTBRACKET:
								console->push(']');

								break;

							case SDLK_BACKSLASH:
								console->push('\\');

								break;

							case SDLK_SEMICOLON:
								console->push(';');

								break;

							case SDLK_QUOTE:
								console->push('\'');

								break;

							case SDLK_COMMA:
								console->push(',');

								break;

							case SDLK_PERIOD:
								console->push('.');

								break;

							case SDLK_SLASH:
								console->push('/');

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

							case SDLK_F5:
								objMv(obj[0], d);

								std::cout << util::phys::aabb(
										obj[0],
										obj[1]
										) << std::endl;

								break;
						}	
					}
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				cam._drag = true;

				SDL_GetMouseState(&cam._begin[0], &cam._begin[1]);

				cam._prev = cam._pos;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				cam._drag = false;
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (cam._drag) {
					SDL_GetMouseState(&cam._curr[0], &cam._curr[1]);

					cam._delta[0] = cam._begin[0] - cam._curr[0];
					cam._delta[1] = cam._begin[1] - cam._curr[1];

					cam._pos[0] = cam._prev.x + cam._delta[0];
					cam._pos[2] = cam._prev.y + cam._delta[1];
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
