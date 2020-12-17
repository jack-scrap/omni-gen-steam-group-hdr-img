#include <vector>
#include <Python.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>

#include "console.h"
#include "col.h"
#include "state.h"

Console::Console(std::vector<std::string> buff) :
	_buff(buff),
	_mode(EDITOR),
	Mesh(util::mesh::plane(glm::vec2(abs(-1 - 1), -1 -1)), "text", "text", glm::vec2(-1.0, 1.0)) {
		// text
		TTF_Init();

		font = TTF_OpenFont("res/terminus.bdf", state::dim[1]);

		SDL_FillRect(_canv, &_canvRect, SDL_MapRGBA(_canv->format, col[false][0], col[false][1], col[false][2], 255));
		SDL_FillRect(_bg, &_bgRect, SDL_MapRGBA(_bg->format, col[true][0], col[true][1], col[true][2], 255));

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

	// highlighting
	for (int l = 0; l < _buff.size(); l++) {
		std::vector<bool> line;

		for (int i = 0; i < _buff[l].size(); i++) {
			line.push_back(false);
		}

		_hl.push_back(line);
	}

	// buffer
	for (int l = 0; l < _buff.size(); l++) {
		std::vector<SDL_Surface*> line;

		for (int i = 0; i < _buff[l].size(); i++) {
			line.push_back(TTF_RenderGlyph_Blended(font, _buff[l][i], {col[!_hl[l][i]][0], col[!_hl[l][i]][1], col[!_hl[l][i]][2]}));
		}

		_map.push_back(line);
	}

	// command-line
	std::vector<SDL_Surface*> line;

	for (int i = 0; i < _cmd.size(); i++) {
		line.push_back(TTF_RenderGlyph_Blended(font, _cmd[i], {col[true][0], col[true][1], col[true][2]}));
	}

	_map.push_back(line);

	// render
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _canv->w, _canv->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, _canv->pixels);
	for (int l = 0; l < _buff.size(); l++) {
		for (int i = 0; i < _buff[l].size(); i++) {
			if (_hl[l][i]) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * state::dim[0], l * state::dim[1], state::dim[0], state::dim[1], GL_BGRA, GL_UNSIGNED_BYTE, _bg->pixels);
			} else {
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * state::dim[0], l * state::dim[1], state::dim[0], state::dim[1], GL_BGRA, GL_UNSIGNED_BYTE, _map[l][i]->pixels);
			}
		}
	}

	for (int i = 0; i < _cmd.size(); i++) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, i * state::dim[0], (state::line - 1) * state::dim[1], state::dim[0], state::dim[1], GL_BGRA, GL_UNSIGNED_BYTE, _map[_map.size() - 1][i]->pixels);
	}

	// cursor
	switch (_mode) {
		case EDITOR:
			_idx[0] = _buff.back().size();
			_idx[1] = _buff.size() - 1;

			break;

		case CMD:
			_idx[0] = _cmd.size();
			_idx[1] = state::line - 1;

			break;
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, _idx[0] * state::dim[0], _idx[1] * state::dim[1], state::dim[0], state::dim[1], GL_BGRA, GL_UNSIGNED_BYTE, _bg->pixels);

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
			_cmd.push_back(c);

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
	if (_cmd == "run") {
		PyRun_SimpleString(util::str::join(_buff).c_str());
	}

	_cmd.clear();

	render();
}

void Console::pop() {
	switch (_mode) {
		case EDITOR:
			if (!_buff.empty()) {
				if (!_buff.back().empty()) {
					_buff.back().pop_back();
				} else {
					_buff.pop_back();
				}
			}

			break;

		case CMD:
			if (!_cmd.empty()) {
				_cmd.pop_back();
			}

			break;
	}

	render();
}

void Console::draw() {
	glBindVertexArray(_id[VAO]);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, _pos.size());

	_prog.unUse();
	glBindVertexArray(0);
}
