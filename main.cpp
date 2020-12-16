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
						case SDLK_RETURN:
							console->newline();

							break;

						case SDLK_BACKSPACE:
							console->pop();

							break;
					}	
				}

				/* if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) { */
				/* 	console1->_buff.back().push_back((char) e.key.keysym.sym); */

				/* 	console1->_bg.back().push_back(Bg(false, { */
				/* 		console1->_bg.back().size(), console1->_bg.size() - 1 */
				/* 	})); */
				/* 	console1->_txt.back().push_back(Char((char) e.key.keysym.sym, true, font, { */
				/* 		console1->_txt.back().size(), console1->_bg.size() - 1 */
				/* 	})); */
				/* } else { */
				/* 	switch (e.key.keysym.sym) { */
				/* 		case SDLK_SPACE: */
				/* 			console1->_buff.back().push_back((char) e.key.keysym.sym); */

				/* 			console1->_bg.back().push_back(Bg(false, { */
				/* 				console1->_bg.back().size(), console1->_bg.size() - 1 */
				/* 			})); */
				/* 			console1->_txt.back().push_back(Char((char) e.key.keysym.sym, true, font, { */
				/* 				console1->_txt.back().size(), console1->_bg.size() - 1 */
				/* 			})); */

				/* 		break; */

				/* 		case SDLK_BACKSPACE: */
				/* 			if (!console1->_buff.empty()) { */
				/* 				if (!console1->_buff.back().empty() &&!console1->_txt.back().empty()) { */
				/* 					console1->_buff.back().pop_back(); */

				/* 					console1->_bg.back().pop_back(); */
				/* 					console1->_txt.back().pop_back(); */
				/* 				} else { */
				/* 					console1->_buff.pop_back(); */

				/* 					console1->_bg.pop_back(); */
				/* 					console1->_txt.pop_back(); */
				/* 				} */
				/* 			} */

				/* 			break; */

				/* 		case SDLK_RETURN: */
				/* 			console1->_buff.push_back(""); */

				/* 			console1->_bg.push_back({}); */
				/* 			console1->_txt.push_back({}); */

				/* 			break; */

				/* 		case SDLK_F1: */
				/* 			console1->_mode = EDITOR; */

				/* 			break; */

				/* 		case SDLK_F2: */
				/* 			console1->_mode = CMD; */

				/* 			break; */

				/* 		case SDLK_F3: */
				/* 			PyRun_SimpleString(util::str::join(console1->_buff).c_str()); */

				/* 			break; */
				/* 	} */
				/* } */
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
