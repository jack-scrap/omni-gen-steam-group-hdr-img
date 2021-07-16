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

bool boot;

void dispatch(Console* console, std::string name) {
	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(name);

	PyRun_SimpleString(util::str::join(buff).c_str());

	bool eq = false;

	if (eq) {
		std::string log = "log/" + util::fs::base(name) + ".log";

		console->_buff = util::log(console->_buff.size(), console->_maxFs);

		nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("stat.json"));

		unsigned int rank = deser["rank"];
		rank++;

		nlohmann::json data = {
			{
				"rank", rank
			}
		};

		std::string serial = data.dump(1, '\t');

		util::fs::write("stat.json", util::str::split(serial, '\n'));
	}
}

void Console::scrub(unsigned int dir) {
	switch (dir) {
		case L:
			if (_curs[MIN][X] > 0) {
				for (int i = 0; i < 2; i++) {
					_curs[i][X]--;
				}
			}

			break;

		case R:
			if (_curs[MIN][X] < _buff[_curs[MIN][Y] - 1].size()) {
				for (int i = 0; i < 2; i++) {
					_curs[i][X]++;
				}
			}

			break;

		case D:
			if (_curs[MIN][Y] < _buff.size()) {
				for (int i = 0; i < 2; i++) {
					_curs[i][X] = 0;
					_curs[i][Y]++;
				}
			}

			break;

		case U:
			if (_curs[MIN][Y] > 0) {
				for (int i = 0; i < 2; i++) {
					_curs[i][X] = 0;
					_curs[i][Y]--;
				}
			}

			break;
	}

	for (int i = 0; i < 2; i++) {
		_curs[i][X] = _maxFs + 1 + _maxNo + 1 + _curs[i][X];
		_curs[i][Y] = _curs[i][Y];
	}
}

Console::Console(std::string cwd, std::string name, std::vector<std::string> buff) :
	_cwd(cwd),
	_tree(util::fs::ls(cwd)),
	_mode(EDITOR),
	_sel(IDX),
	_prog("text", "text") {
		if (buff.size()) {
			_buff = buff;
		} else {
			_buff = {
				""
			};
		}

		_name = name;

		_w = util::fs::perm(_name)[1] == 'w';

		if (!_w) {
			std::string post;
			post += " ";
			post += "[RO]";

			_name += post;
		}

		_scr = (char*) malloc(state::line * state::ln *sizeof (char));
		_hl = (bool*) calloc(state::line * state::ln, sizeof (bool));

		// file system
		std::sort(_tree.begin(), _tree.end());

		for (std::map<std::string, std::string> _ : _tree) {
			if (_["name"].size() > _maxFs) {
				_maxFs = _["name"].size();
			}
		}

		_maxNo = std::to_string(_buff.size()).size();

		// index
		switch (_mode) {
			case FS:
				_curs[MIN][X] = 0;
				_curs[MIN][Y] = 1 + _l;
				_curs[MAX][X] = 0;
				_curs[MAX][Y] = 1 + _l;

				break;

			case EDITOR:
				_curs[MIN][X] = _maxFs + 1 + _maxNo + 1 + _buff.back().size();
				_curs[MIN][Y] = 1 + _buff.size() - 1;
				_curs[MAX][X] = _maxFs + 1 + _maxNo + 1 + _buff.back().size();
				_curs[MAX][Y] = 1 + _buff.size() - 1;

				break;

			case PROMPT:
				_curs[MIN][X] = (_ps1 + _prompt).size();
				_curs[MIN][Y] = state::line - 1;
				_curs[MAX][X] = (_ps1 + _prompt).size();
				_curs[MAX][Y] = state::line - 1;

				break;
		}

		/* data */
		glGenVertexArrays(1, &_id[Console::VAO]);
		glBindVertexArray(_id[Console::VAO]);

		glGenBuffers(2, &_id[1]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[Console::VBO]);
		std::vector<GLfloat> pos = util::mesh::plane(glm::vec2(abs(-1 - 1), -1 - 1));
		glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof (GLfloat), &pos[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, _id[2]);
		glBufferData(GL_ARRAY_BUFFER, _st.size() * sizeof (GLfloat), &_st[0], GL_STATIC_DRAW);

		/* shader */
		_prog.use();

		// attribute
		glBindBuffer(GL_ARRAY_BUFFER, _id[Console::VBO]);
		_attr[Console::POS] = glGetAttribLocation(_prog._id, "pos");
		glVertexAttribPointer(_attr[POS], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[POS]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[Console::STBO]);
		_attr[Console::ST] = glGetAttribLocation(_prog._id, "st");
		glVertexAttribPointer(_attr[ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[ST]);

		// text
		TTF_Init();

		font = TTF_OpenFont("res/terminus.bdf", layout::dim[Y]);

		SDL_FillRect(_canv, &_canvRect, SDL_MapRGBA(_canv->format, col[false][R], col[false][G], col[false][B], 255));
		SDL_FillRect(_block, &_blockRect, SDL_MapRGBA(_block->format, col[true][R], col[true][G], col[true][B], 255));

		// OpenGL
		glGenTextures(1, &_tex);

		render();

		// Python
		Py_Initialize();

		// path
		PyObject* path = PySys_GetObject("path");
		PyList_Append(path, PyUnicode_FromString("."));
	}

void Console::render() {
	std::string modeStr;
	switch (_mode) {
		case FS:
			modeStr = "FS";

			break;

		case EDITOR:
			modeStr = "EDITOR";

			break;

		case PROMPT:
			modeStr = "PROMPT";

			break;
	};

	std::string status;

	std::string time = util::now(state::format);
	status += util::str::pad(time, time.size() + 1);

	std::string base = util::fs::name(_name);
	status += util::str::pad(base, base.size() + 1);

	std::string statusPadded = util::str::pad(status, state::ln - modeStr.size());
	statusPadded += modeStr;

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
	if (_buff.size() < state::line - 1 - 1) {
		roof = _buff.size();
	} else {
		roof = state::line - 1 - 1;
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

	/* highlighting */
	// line
	int dir = 0;
	if (_curs[MAX][X] > _curs[MIN][X]) {
		dir = 1;
	}
	if (_curs[MAX][X] < _curs[MIN][X]) {
		dir = -1;
	}

	unsigned int
		l = _curs[MIN][Y],
		delta = abs((int) _curs[MAX][X] - (int) _curs[MIN][X]);
	for (int i = _curs[MIN][X]; i < _curs[MIN][X] + delta; i++) {
		_hl[(l * state::ln) + (i * dir)] = true;
	}

	// line number
	if (state::hlLineNo) {
		for (int i = 0; i < _maxNo + 1; i++) {
			_hl[_maxFs + 1 + (l * state::ln) + i] = true;
		}
	}

	glActiveTexture(_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);

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

	if (
		_curs[MIN][X] < state::ln &&
		_curs[MIN][Y] < state::line &&
		_curs[MAX][X] < state::ln &&
		_curs[MAX][Y] < state::line
	) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, _curs[MIN][X] * layout::dim[X], _curs[MIN][Y] * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);
		SDL_FillRect(_block, &_blockRect, SDL_MapRGBA(_block->format, col[true][R], col[true][G], col[true][B], 255));

		glTexSubImage2D(GL_TEXTURE_2D, 0, _curs[MAX][X] * layout::dim[X], _curs[MAX][Y] * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);
		SDL_FillRect(_block, &_blockRect, SDL_MapRGBA(_block->format, col[true][R], col[true][G], col[true][B], 255));
	}

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

			for (int i = 0; i < 2; i++) {
				_curs[i][X] = _maxFs + 1 + _maxNo + 1 + _buff.back().size();
				_curs[i][Y] = _buff.size();
			}

			break;

		case PROMPT:
			_prompt.push_back(c);

			for (int i = 0; i < 2; i++) {
				_curs[i][X] = _ps1.size() + _prompt.size();
				_curs[i][Y] = state::line - 1;
			}

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

	for (int i = 0; i < 2; i++) {
		_curs[i][X] = _maxFs + 1 + _maxNo + 1;
		_curs[i][Y] = 1 + _buff.size() - 1;
	}

	render();
}

void Console::exec() {
	std::vector<std::string> tok = util::str::split(_prompt, ' ');

	_prompt.clear();

	if (tok.size()) {
		std::string _cmd = tok[0];

		if (std::find(omni::lib.begin(), omni::lib.end(), _cmd) != omni::lib.end()) {
			if (_cmd == "open") {
				if (tok.size() > 1) {
					_buff = util::fs::rd<std::vector<std::string>>(tok[1]);
				}
			}

			if (_cmd == "set") {
				if (tok.size() == 1 + 2) {
					_buff = util::fs::rd<std::vector<std::string>>(tok[1]);

					scn::init(tok[1], std::stoi(tok[2]));
				} else {
					omni::err("Incorrect number of arguments");
				}
			}

			if (_cmd == "save") {
				if (tok.size() > 1) {
					util::fs::write(tok[1], _buff);
				} else {
					util::fs::write(_name, _buff);
				}

				_prompt = _name + " written";
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

			if (_cmd == "next") {
				if (eq) {
					eq = false;

					nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("stat.json"));

					unsigned int rank = deser["rank"];

					_buff = util::fs::rd<std::vector<std::string>>("player/script/init/" + std::to_string(rank) + "/" + "main.py");
					scn::init(0, rank);
				} else {
					omni::err("Current level not complete");
				}

				_prompt = "Loaded stage `" + std::string("init") + "`, level `" + std::to_string(0) + "`";
			}

			if (_cmd == "update") {
				if (std::find(omni::var.begin(), omni::var.end(), _cmd) != omni::var.end()) {
					if (tok.size() == 1 + 2) {
						if (tok[1] == "skip_boot") {
							boot = !(tok[2] == "y");
						}

						if (tok[1] == "speed") {
							state::speed = std::stoi(tok[2]);
						}

						if (tok[1] == "wd") {
							layout::view[X] = std::stoi(tok[2]);
							layout::view[Y] = state::line * layout::dim[Y];
						}

						if (tok[1] == "fps") {
							state::fps = std::stoi(tok[2]);
						}

						if (tok[1] == "line_count") {
							state::line = std::stoi(tok[2]);
						}

						if (tok[1] == "line_width") {
							state::ln = std::stoi(tok[2]);
						}

						if (tok[1] == "tab_wd") {
							state::tabWd = std::stoi(tok[2]);
						}

						if (tok[1] == "time_format") {
							state::format = tok[2];
						}
					} else {
						omni::err("Incorrect number of arguments");
					}
				} else {
					omni::err("Variable `" + tok[1] + "` not found");
				}
			}

			if (_cmd == "quit") {
				std::vector<std::string> buff =  util::fs::rd<std::vector<std::string>>(_name);

				bool diff = false;

				if (buff.size() != _buff.size()) {
					diff = true;
				}

				if (!diff) {
					for (int i = 0; i < _buff.size(); i++) {
						if (_buff[i] != buff[i]) {
							diff = true;

							break;
						}
					}
				}

				if (!diff) {
					exit(0);
				} else {
					omni::err("Buffer not saved");
				}
			}
		} else {
			omni::err("Command `" + _cmd + "` not found");
		}

		for (int i = 0; i < 2; i++) {
			_curs[i][X] = (_ps1 + _prompt).size();
			_curs[i][Y] = state::line - 1;
		}

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

			for (int i = 0; i < 2; i++) {
				_curs[i][X] = _maxFs + 1 + _maxNo + 1 + _buff.back().size();
				_curs[i][Y] = _buff.size();
			}

			break;

		case PROMPT:
			if (!_prompt.empty()) {
				_prompt.pop_back();
			}

			for (int i = 0; i < 2; i++) {
				_curs[i][X] = (_ps1 + _prompt).size();
				_curs[i][Y] = state::line - 1;
			}

			break;
	}

	render();
}

void Console::draw() {
	glBindVertexArray(_id[Console::VAO]);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * 2 * 3);

	_prog.unUse();
	glBindVertexArray(0);
}
