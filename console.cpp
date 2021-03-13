#include <thread>
#include <vector>
#include <fstream>
#include <Python.h>
#include <iostream>
#include <algorithm>
#include <SDL2/SDL_ttf.h>

#include "console.h"
#include "col.h"
#include "state.h"
#include "scn.h"

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
		rank++;

		console->_name.clear();
		console->_buff = util::log(state::ln, console->_buff.size());
	}
}

Console::Console(std::string name, std::vector<std::string> buff) :
	_name(name),
	_buff(buff),
	_mode(EDITOR),
	_prog("text", "text") {
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
	}

void Console::render() {
	_hl.clear();
	_map.clear();
	_no.clear();

	unsigned int roof;
	if (_buff.size() < state::line - 2) {
		roof = _buff.size();
	} else {
		roof = state::line - 2;
	}

	// highlighting
	for (int l = 0; l < _buff.size(); l++) {
		std::vector<bool> line;

		for (int i = 0; i < _buff[l].size(); i++) {
			line.push_back(false);
		}

		_hl.push_back(line);
	}

	// buffer
	for (int l = 0; l < roof; l++) {
		std::vector<SDL_Surface*> line;

		for (int i = 0; i < _buff[l].size(); i++) {
			line.push_back(TTF_RenderGlyph_Blended(font, _buff[l][i], {col[!_hl[l][i]][R], col[!_hl[l][i]][G], col[!_hl[l][i]][B]}));
		}

		_map.push_back(line);
	}

	// line numbers
	unsigned int max = std::to_string(_buff.size()).size() + 1;

	for (int l = 0; l < roof; l++) {
		std::vector<SDL_Surface*> line;

		std::string str = std::to_string(1 + l);
		for (int i = str.size(); i < max; i++) {
			str.push_back(' ');
		}
		str.push_back(' ');

		for (int i = 0; i < max; i++) {
			line.push_back(TTF_RenderGlyph_Blended(font, (char) str[i], {col[true][R], col[true][G], col[true][B]}));
		}

		_no.push_back(line);
	}

	// command-line
	std::string prompt = _ps1 + _prompt;
	
	std::vector<SDL_Surface*> line;

	for (int i = 0; i < prompt.size(); i++) {
		line.push_back(TTF_RenderGlyph_Blended(font, prompt[i], {col[true][R], col[true][G], col[true][B]}));
	}

	_map.push_back(line);

	/* render */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _canv->w, _canv->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, _canv->pixels);

	for (int i = 0; i < state::ln; i++) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, i * layout::dim[X], 0, layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);
	}

	for (int l = 0; l < roof; l++) {
		for (int i = 0; i < _no[l].size(); i++) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, i * layout::dim[X], (1 + l) * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _no[l][i]->pixels);
		}
	}

	for (int l = 0; l < roof; l++) {
		for (int i = 0; i < _buff[l].size(); i++) {
			if (_hl[l][i]) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, (max + i) * layout::dim[X], (1 + l) * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);
			} else {
				glTexSubImage2D(GL_TEXTURE_2D, 0, (max + i) * layout::dim[X], (1 + l) * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _map[l][i]->pixels);
			}
		}
	}

	for (int i = 0; i < prompt.size(); i++) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, i * layout::dim[X], (state::line - 1) * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _map[_map.size() - 1][i]->pixels);
	}

	// cursor
	switch (_mode) {
		case EDITOR:
			_idx[X] = max + _buff.back().size();
			_idx[Y] = _buff.size();

			break;

		case CMD:
			_idx[X] = prompt.size();
			_idx[Y] = state::line - 1;

			break;
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, _idx[X] * layout::dim[X], _idx[Y] * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGRA, GL_UNSIGNED_BYTE, _block->pixels);

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

		case CMD:
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

		case CMD:
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

		std::vector<std::string> _lib = {
			"open",
			"run",
			"save",
			"next"
		};
		if (std::find(_lib.begin(), _lib.end(), _cmd) != _lib.end()) {
			if (_cmd == "open") {
				if (tok.size() > 1) {
					open(tok[1]);
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
					save(tok[1]);
				} else {
					save(_name);
				}
			}

			if (_cmd == "next") {
				_buff = util::fs::rd<std::vector<std::string>>("script/" + std::to_string(rank) + ".py");
				ld(rank);
			}
		} else {
			std::cout << "Error: Command `" + _cmd + "` not found" << std::endl;
		}

		_prompt.clear();

		render();
	}
}

void Console::open(std::string name) {
	_buff = util::fs::rd<std::vector<std::string>>(name);
}

void Console::save(std::string name) {
	std::ofstream f;
	f.open(name);

	for (const std::string& l : _buff) {
		f << l + '\n';
	}

	f.close();
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

		case CMD:
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
