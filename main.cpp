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

Console* console;

int main() {
	unsigned int lvl = 0;
	console = new Console("script/" + std::to_string(lvl) + ".py", util::fs::rd<std::vector<std::string>>("script/" + std::to_string(lvl) + ".py"));
	scn::ld(lvl);

	SDL_Event e;
	while (disp._open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				// modifier
				if (e.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
					if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
						console->push((char) (e.key.keysym.sym - 32));
					} else if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
						char c;
						switch (e.key.keysym.sym) {
							case SDLK_1:
								c = '!';

								break;

							case SDLK_2:
								c = '@';

								break;

							case SDLK_3:
								c = '#';

								break;

							case SDLK_4:
								c = '$';

								break;

							case SDLK_5:
								c = '%';

								break;

							case SDLK_6:
								c = '^';

								break;

							case SDLK_7:
								c = '&';

								break;

							case SDLK_8:
								c = '*';

								break;

							case SDLK_9:
								c = '(';

								break;

							case SDLK_0:
								c = ')';

								break;
						}

						console->push((char) c);
					} else {
						switch (e.key.keysym.sym) {
							case SDLK_BACKQUOTE:
								console->push('~');

								break;

							case SDLK_MINUS:
								console->push('_');

								break;

							case SDLK_EQUALS:
								console->push('+');

								break;

								// bracket
							case SDLK_LEFTBRACKET:
								console->push('{');

								break;

							case SDLK_RIGHTBRACKET:
								console->push('}');

								break;

							case SDLK_BACKSLASH:
								console->push('|');

								break;

							case SDLK_SEMICOLON:
								console->push(':');

								break;

							case SDLK_QUOTE:
								console->push('"');

								break;

							case SDLK_COMMA:
								console->push('<');

								break;

							case SDLK_PERIOD:
								console->push('>');

								break;

							case SDLK_SLASH:
								console->push('?');

								break;
						}
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

							case SDLK_TAB:
								for (int i = 0; i < state::tabWd; i++) {
									console->push(' ');
								}

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
						}	
					}
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				SDL_GetMouseState(&cam._begin[0], &cam._begin[1]);

				cam._drag = true;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				cam._drag = false;

				cam._prev = cam._pos;
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (cam._drag) {
					SDL_GetMouseState(&cam._curr[0], &cam._curr[1]);

					cam._delta[0] = cam._curr[0] - cam._begin[0];
					cam._delta[1] = -(cam._curr[1] - cam._begin[1]);

					glm::vec2 proj = glm::vec2(
							cam._prev[0] - cam._delta[0] + cam._delta[1] + 1000.0,
							cam._prev[2] + cam._delta[0] + cam._delta[1] + 1000.0
							);

					glm::vec2 bound[2] = {
						glm::vec2(-400, 400),
						glm::vec2(-400, 400)
					};

					if (
							proj[0] > bound[0][0] &&
							proj[0] < bound[0][1] &&

							proj[1] > bound[1][0] &&
							proj[1] < bound[1][1]
						 ) {
						cam._pos[0] = cam._prev[0] - cam._delta[0] + cam._delta[1];
						cam._pos[2] = -(cam._prev[2] + cam._delta[0] + cam._delta[1]);
					}
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
