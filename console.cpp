#include <thread>
#include <vector>
#include <fstream>
#include <Python.h>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <SDL2/SDL_ttf.h>

#include "console.h"
#include "col.h"
#include "state.h"
#include "scn.h"
#include "omni.h"

void dispatch(Console* console, std::string name) {
	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(name);

	PyRun_SimpleString(util::str::join(buff).c_str());

	char lhs[data->_x];
	for (int i = 0; i < data->_x; i++) {
		if (data->_data[i]->_data) {
			lhs[i] = data->_data[i]->_data->_c;
		} else {
			lhs[i] = 0;
		}
	}

	eq = true;
	for (int i = 0; i < data->_x; i++) {
		if (lhs[i] != rhs[i]) {
			eq = false;
		}
	}

	if (eq) {
		std::string log = "log/" + std::to_string(rank) + ".log";

		util::fs::write(log, util::log(state::ln, console->_buff.size()));

		console->_name.clear();
		console->_buff = util::fs::rd<std::vector<std::string>>(log);

		rank++;
	}
}

Console::Console(std::string name, std::vector<std::string> buff) :
	_tree(util::fs::ls("script")),
	_name(name),
	_buff(buff),
	_mode(EDITOR),
	_prog("text", "text") {
		std::sort(_tree.begin(), _tree.end());

		// status bar
		_scr.push_back(util::str::pad("asdf", state::ln));

		// file system
		for (std::map<std::string, std::string> _ : _tree) {
			if (_["name"].size() > maxFs) {
				maxFs = _["name"].size();
			}
		}

		// line numbers
		unsigned int roof;
		if (_buff.size() < state::line - 2) {
			roof = _buff.size();
		} else {
			roof = state::line - 2;
		}

		for (int i = 0; i < roof; i++) {
			std::string str = std::to_string(i);

			if (str.size() > maxLn) {
				maxLn = str.size();
			}
		}

		// screen
		for (int l = 0; l < _buff.size(); l++) {
			std::string line;

			// file system
			if (l < _tree.size()) {
				std::string fs = util::str::pad(_tree[l]["name"], maxFs);
				for (int i = 0; i < fs.size(); i++) {
					line.push_back(fs[i]);
				}
			} else {
				for (int i = 0; i < maxFs; i++) {
					line.push_back(' ');
				}
			}

			// line numbers
			line.push_back(' ');

			std::string no = std::to_string(l + 1);
			for (int i = 0; i < no.size(); i++) {
				line.push_back(no[i]);
			}

			// buffer
			line.push_back(' ');

			unsigned int curr = line.size();

			std::string padded = util::str::pad(_buff[l], state::ln);
			for (int i = 0; i < state::ln - curr; i++) {
				line.push_back(padded[i]);
			}

			_scr.push_back(line);
		}

		for (int l = _scr.size(); l < state::line - 1; l++) {
			_scr.push_back(util::str::pad("", state::ln));
		}

		// command-line
		_scr.push_back(util::str::pad(_prompt, state::ln));

		/* data */
		glGenVertexArrays(1, &_id[VAO]);
		glBindVertexArray(_id[VAO]);

		glGenBuffers(2, &_id[1]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[VBO]);
		std::vector<GLfloat> pos = util::mesh::plane(glm::vec2(abs(-1 - 1), -1 - 1));
		glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof (GLfloat), &pos[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, _id[STBO]);
		glBufferData(GL_ARRAY_BUFFER, _st.size() * sizeof (GLfloat), &_st[0], GL_STATIC_DRAW);

		/* shader */
		_prog.use();

		// attribute
		glBindBuffer(GL_ARRAY_BUFFER, _id[VBO]);
		_attr[POS] = glGetAttribLocation(_prog.id, "pos");
		glVertexAttribPointer(_attr[POS], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[POS]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[STBO]);
		_attr[ST] = glGetAttribLocation(_prog.id, "st");
		glVertexAttribPointer(_attr[ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[ST]);

		// text
		TTF_Init();

		font = TTF_OpenFont("res/terminus.bdf", layout::dim[Y]);

		SDL_FillRect(_canv, &_canvRect, SDL_MapRGBA(_canv->format, col[false][R], col[false][G], col[false][B], 255));
		SDL_FillRect(_block, &_blockRect, SDL_MapRGBA(_block->format, col[true][R], col[true][G], col[true][B], 255));

		// OpenGL
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		render();

		// Python
		Py_Initialize();

		// path
		PyObject* path = PySys_GetObject("path");
		PyList_Append(path, PyUnicode_FromString("."));
	}

void Console::render() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _canv->w, _canv->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, _canv->pixels);

	for (int l = 0; l < _scr.size(); l++) {
		for (int i = 0; i < _scr[l].size(); i++) {
			SDL_Surface* surf = TTF_RenderGlyph_Blended(font, _scr[l][i], {col[true][R], col[true][G], col[true][B]});
			glTexSubImage2D(GL_TEXTURE_2D, 0, i * layout::dim[X], l * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
		}
	}

	// cursor
	switch (_mode) {
		case FS:
			_idx[X] = 0;
			_idx[Y] = 1 + _l;

			break;

		case EDITOR:
			_idx[X] = maxFs + 1 + maxLn + 1 + _buff[_buff.size() - 1].size();
			_idx[Y] = 1 + _buff.size() - 1;

			break;

		case PROMPT:
			_idx[X] = _prompt.size();
			_idx[Y] = state::line - 1;

			break;
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, _idx[X] * layout::dim[X], _idx[Y] * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);
	SDL_FillRect(_block, &_blockRect, SDL_MapRGBA(_block->format, col[true][R], col[true][G], col[true][B], 255));

	glGenerateMipmap(GL_TEXTURE_2D);
}

void Console::push(char c) {
	switch (_mode) {
		case EDITOR:
			if (!_buff.empty()) {
				_buff.back().push_back(c);
			} else {
				_buff.push_back({});
				_buff.back().push_back(c);
			}

			break;

		case PROMPT:
			_prompt.push_back(c);

			break;
	}

	render();
}

void Console::enter() {
	switch (_mode) {
		case EDITOR:
			newline();

			break;

		case PROMPT:
			exec();

			break;
	}
}

void Console::newline() {
	_buff.push_back({});

	render();
}

void Console::exec() {
	std::vector<std::string> tok = util::str::split(_prompt, ' ');

	if (tok.size()) {
		std::string _cmd = tok[0];

		if (std::find(omni::lib.begin(), omni::lib.end(), _cmd) != omni::lib.end()) {
			if (_cmd == "open") {
				if (tok.size() > 1) {
					_buff = util::fs::rd<std::vector<std::string>>(tok[1]);
				}
			}

			if (_cmd == "run") {
				if (tok.size() > 1) {
					std::thread t(dispatch, this, tok[1]);
					t.detach();
				} else {
					std::thread t(dispatch, this, _name);
					t.detach();
				}
			}

			if (_cmd == "save") {
				if (tok.size() > 1) {
					util::fs::write(tok[1], _buff);
				} else {
					util::fs::write(_name, _buff);
				}
			}

			if (_cmd == "next") {
				if (eq) {
					eq = false;

					_buff = util::fs::rd<std::vector<std::string>>("script/" + std::to_string(rank) + ".py");
					scn::init(rank);
				} else {
					omni::err("Current level not complete");
				}
			}
		} else {
			omni::err("Command `" + _cmd + "` not found");
		}

		_prompt.clear();

		render();
	}
}

void Console::pop() {
	switch (_mode) {
		case EDITOR:
			if (!_buff.empty()) {
				if (!_buff.back().empty()) {
					_buff.back().pop_back();
				} else {
					if (_buff.size() > 1) {
						_buff.pop_back();
					}
				}
			}

			break;

		case PROMPT:
			if (!_prompt.empty()) {
				_prompt.pop_back();
			}

			break;
	}

	render();
}

void Console::draw() {
	glBindVertexArray(_id[VAO]);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * 2 * 3);

	_prog.unUse();
	glBindVertexArray(0);
}
