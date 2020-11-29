#include <Python.h>

#include "console.h"
#include "state.h"

Console::Console(std::vector<std::string> buff, unsigned int ln, glm::vec2 scr) {
	TTF_Init();

	TTF_Font* font = TTF_OpenFont("res/terminus.bdf", state::sz[1]);

	for (int l = 0; l < buff.size(); l++) {
		std::vector<Bg> str;

		for (int i = 0; i < buff[l].size(); i++) {
			str.push_back(Bg(false, {
				i, l
			}));
		}

		_bg.push_back(str);
	}

	for (int l = 0; l < buff.size(); l++) {
		std::vector<Char> str;

		for (int i = 0; i < buff[l].size(); i++) {
			str.push_back(Char(buff[l][i], true, font, {
				i, l
			}));
		}

		_buff.push_back(str);
	}

	// dimensions
	_res = glm::vec2(ln * state::sz[0], scr[1]);

	// Python
	Py_Initialize();
}

void Console::print() {
	glDisable(GL_DEPTH_TEST);

	for (int l = 0; l < _buff.size(); l++) {
		for (int i = 0; i < _buff[l].size(); i++) {
			_bg[l][i].draw();
			_buff[l][i].draw();
		}
	}
}
