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

int main() {
	Console* console = new Console(util::fs::rd<std::vector<std::string>>("script/asdf.py"));

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

			if (e.type == SDL_QUIT) {
				disp._open = false;
			}
		}

		disp.clear();

		glEnable(GL_DEPTH_TEST);

		glViewport(state::res[X], 0, state::view[X], state::view[Y]);

		truckDraw(truck);

		glViewport(0, 0, state::res[Y], state::view[Y]);

		console->draw();

		disp.update();
	}

	return 0;
}
