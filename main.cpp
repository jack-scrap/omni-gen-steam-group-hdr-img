#include <iostream>
#include <Python.h>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "scn.h"
#include "util.h"
#include "state.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "omni.h"
#include "col.h"

Disp* disp;
Console* console;

Cam cam = {
	{
		-1000.0, 1000.0, 1000.0
	}, {
		50, 50, 50
	}, {
		-1000.0, 1000.0, -1000.0
	}
};

int main(int argc, char** argv) {
	// initialize
	std::map<std::string, int> data = util::cfg::parse("cfg/init.cfg");

	for (const auto& [key, val] : data) {
		if (key == "wd") {
			layout::view = glm::vec2(val, state::line * layout::dim[Y]);
		}

		if (key == "fps") {
			state::fps = val;
		}

		if (key == "line_count") {
			state::line = val;
		}

		if (key == "line_width") {
			state::ln = val;
		}

		if (key == "tab_wd") {
			state::tabWd = val;
		}
	}

	layout::res = glm::vec2(state::ln * layout::dim[X], layout::view[Y]);

	disp = new Disp("Omni", {
		layout::view[X] + (state::ln * layout::dim[X]), layout::view[Y]
	}, col[false]);

	unsigned int stage = 0;
	unsigned int lvl;
	if (argv[1]) {
		lvl = atoi(argv[1]);
	} else {
		lvl = 0;
	}
	std::string name = "script/" + omni::stage[stage] + "/" + std::to_string(lvl) + "/main.py";
	console = new Console(name, util::fs::rd<std::vector<std::string>>(name));
	scn::init(stage, lvl);

	SDL_Event e;
	while (disp->_open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_F1:
						console->_mode = Console::FS;

						break;

					case SDLK_F2:
						console->_mode = Console::EDITOR;

						console->render();

						break;

					case SDLK_F3:
						console->_mode = Console::PROMPT;

						console->render();

						break;
				}

				switch (console->_mode) {
					case Console::FS:
						console->_modeStr = "FS";

						break;

					case Console::EDITOR:
						console->_modeStr = "EDITOR";

						break;

					case Console::PROMPT:
						console->_modeStr = "PROMPT";

						break;
				};

				switch (console->_mode) {
					case Console::FS:
						switch (e.key.keysym.sym) {
							case SDLK_DOWN:
								if (console->_l < console->_tree.size() - 1) {
									console->_l++;
								}

								break;

							case SDLK_UP:
								if (console->_l > 0) {
									console->_l--;
								}

								break;
						}

						for (int l = 0; l < console->_tree.size(); l++) {
							for (int i = 0; i < console->_maxFs; i++) {
								console->_hl[((1 + l) * state::ln) + i] = false;
							}
						}

						for (int i = 0; i < console->_maxFs; i++) {
							console->_hl[((1 + console->_l) * state::ln) + i] = true;
						}

						console->_idx[X] = std::to_string(console->_buff.size()).size() + 1;
						console->_idx[Y] = 1 + console->_l;

						break;

					case Console::EDITOR:
					case Console::PROMPT:
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
								}
							}
						}

						break;
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
				disp->_open = false;
			}
		}

		disp->draw();
	}

	return 0;
}
