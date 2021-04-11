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
		std::string log = "log/" + util::fs::base(name) + ".log";

		console->_buff = util::log(console->_buff.size());

		nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("player.json"));

		unsigned int rank = serial["rank"];
		rank++;

		nlohmann::json data = {
			{
				"rank", rank
			}
		};

		std::string deser = data.dump();

		util::fs::write("player.json", util::str::split(deser, '\n'));
	}
}

Console::Console(std::string name, std::vector<std::string> buff) :
	_tree(util::fs::ls("script")),
	_mode(EDITOR),
	_prog("text", "text") {
		if (buff.size()) {
			_buff = buff;
		} else {
			_buff = {
				""
			};
		}

		_name = name;

		_w = util::fs::w(_name);

		if (!_w) {
			std::string post;
			post += " ";
			post += "[RO]";

			_name += post;
		}

		_scr = (char*) malloc(sizeof (char) * state::line * state::ln);
		_hl = (bool*) malloc(sizeof (bool) * state::line * state::ln);

		// highlighting
		for (int l = 0; l < state::line; l++) {
			for (int i = 0; i < state::ln; i++) {
				_hl[(l * state::ln) + i] = false;
			}
		}

		// file system
		std::sort(_tree.begin(), _tree.end());

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

		switch (_mode) {
			case FS:
				_idx[X] = 0;
				_idx[Y] = 1 + _l;

				break;

			case EDITOR:
				_idx[X] = _maxFs + 1 + _maxNo + 1 + _buff[_buff.size() - 1].size();
				_idx[Y] = 1 + _buff.size() - 1;

				break;

			case PROMPT:
				_idx[X] = (_ps1 + _prompt).size();
				_idx[Y] = state::line - 1;

				break;
		}
	}

void Console::render() {
	switch (_mode) {
		case FS:
			_modeStr = "FS";

			break;

		case EDITOR:
			_modeStr = "EDITOR";

			break;

		case PROMPT:
			_modeStr = "PROMPT";

			break;
	};

	std::string status;

	std::string time = util::now("%Y-%m-%d");
	status += util::str::pad(time, time.size() + 1);

	std::string base = util::fs::name(_name);
	status += util::str::pad(base, base.size() + 1);

	std::string statusPadded = util::str::pad(status, state::ln - _modeStr.size());
	statusPadded += _modeStr;

	for (int i = 0; i < state::ln; i++) {
		_scr[i] = statusPadded[i];
	}

	for (std::map<std::string, std::string> _ : _tree) {
		if (_["name"].size() > _maxFs) {
			_maxFs = _["name"].size();
		}
	}

	_maxNo = std::to_string(_buff.size()).size();

	unsigned int roof;
	if (_buff.size() < state::line - 2) {
		roof = _buff.size();
	} else {
		roof = state::line - 2;
	}

	for (int l = 0; l < state::line - 1 - 1; l++) {
		std::string line;

		// file system
		std::string entry;
		if (l < _tree.size()) {
			entry = _tree[l]["name"];
		} else {
			entry = "";
		}
		std::string entryPadded = util::str::pad(entry, _maxFs + 1);

		line += entryPadded;

		// line numbers
		std::string no;
		if (l < _buff.size()) {
			no = std::to_string(l + 1);
		} else {
			no = "";
		}
		std::string noPadded = util::str::pad(no, _maxNo + 1);

		line += noPadded;

		// buffer
		std::string str;
		if (l < _buff.size()) {
			str = _buff[l];
		} else {
			str = "";
		}
		std::string strPadded = util::str::pad(str, state::ln);

		line += strPadded;

		int i = 0;
		int c = 0;
		while (i < state::ln) {
			if (line[c] == '\t') {
				int t = 0;
				while (
					t < state::tabWd &&
					i < state::ln
				) {
					_scr[((1 + l) * state::ln) + i] = ' ';

					i++;
					t++;
				}

				c++;
			} else {
				_scr[((1 + l) * state::ln) + i] = line[c];

				i++;
				c++;
			}
		}
	}

	// command-line
	std::string str = _ps1 + _prompt;
	std::string strPadded = util::str::pad(str, state::ln);

	for (int i = 0; i < state::ln; i++) {
		_scr[((state::line - 1) * state::ln) + i] = strPadded[i];
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _canv->w, _canv->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, _canv->pixels);

	for (int l = 0; l < state::line; l++) {
		for (int i = 0; i < state::ln; i++) {
			if (_hl[(l * state::ln) + i]) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * layout::dim[X], l * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);
			} else {
				SDL_Surface* surf = TTF_RenderGlyph_Blended(font, _scr[(l * state::ln) + i], {col[true][R], col[true][G], col[true][B]});
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * layout::dim[X], l * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
			}
		}
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

	_idx[X]++;

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

	_idx[X] = _maxFs + 1 + _maxNo + 1;
	_idx[Y] = 1 + _buff.size() - 1;

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

					nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("player.json"));

					unsigned int rank = serial["rank"];

					_buff = util::fs::rd<std::vector<std::string>>("script/" + omni::stage[0] + "/" + std::to_string(rank) + "/" + "main.py");
					scn::init(0, rank);
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
