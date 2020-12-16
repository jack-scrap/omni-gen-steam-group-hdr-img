#include <Python.h>

#include "console.h"
#include "state.h"

Console::Console(std::vector<std::string> buff, unsigned int ln, glm::vec2 scr) :
	_buff(buff),
	_cursor(true, {
		buff.back().size(), buff.size() - 1
	}) {
		// font
		TTF_Init();

		TTF_Font* font = TTF_OpenFont("res/terminus.bdf", state::sz[1]);

		if (buff.size() < state::l) {
			roof = buff.size();
		} else {
			roof = state::l;
		}

		for (int l = 0; l < roof; l++) {
			std::vector<Bg> bg;
			std::vector<Char> str;

			for (int i = 0; i < buff[l].size(); i++) {
				bg.push_back(Bg(false, {
					i, l
				}));
				str.push_back(Char(buff[l][i], true, font, {
					i, l
				}));
			}

			_bg.push_back(bg);
			_txt.push_back(str);
		}

		// command-line
		std::vector<Bg> bg;
		std::vector<Char> str;

		for (int i = 0; i < cmd.size(); i++) {
			bg.push_back(Bg(false, {
				i, state::l
			}));
			str.push_back(Char(cmd[i], true, font, {
				i, state::l
			}));
		}

		_bg.push_back(bg);
		_txt.push_back(str);

		// dimensions
		_res = glm::vec2(ln * state::sz[0], scr[1]);

		// Python
		Py_Initialize();
	}

void Console::print() {
	_cursor = Bg(true, {
		_buff.back().size(), _buff.size() - 1
	});

	glDisable(GL_DEPTH_TEST);

	for (int l = 0; l < roof; l++) {
		for (int i = 0; i < _buff[l].size(); i++) {
			_bg[l][i].draw();
			_txt[l][i].draw();
		}
	}

	for (int i = 0; i < cmd.size(); i++) {
		_bg[_buff.size()][i].draw();
		_txt[_buff.size()][i].draw();
	}

	_cursor.draw();
}
