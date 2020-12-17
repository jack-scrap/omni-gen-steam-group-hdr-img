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

		font = TTF_OpenFont("res/terminus.bdf", state::sz[1]);

		// OpenGL
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		render();

		// Python
		Py_Initialize();
	}

void Console::render() {
	SDL_Surface* canv = SDL_CreateRGBSurface(0, state::res[0], state::res[1], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_FillRect(canv, &canvRect, SDL_MapRGBA(canv->format, col[false][0], col[false][1], col[false][2], 255));

	/* SDL_Surface* bg = SDL_CreateRGBSurface(0, state::sz[0], state::sz[1], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); */
	/* SDL_FillRect(bg, &bgRect, SDL_MapRGBA(canv->format, col[false][0], col[false][1], col[false][2], 255)); */

	_hl.clear();
	_map.clear();

	for (int l = 0; l < _buff.size(); l++) {
		std::vector<bool> line;

		for (int i = 0; i < _buff[l].size(); i++) {
			line.push_back(i > 2);
		}

		_hl.push_back(line);
	}

	for (int l = 0; l < _buff.size(); l++) {
		std::vector<SDL_Surface*> line;

		for (int i = 0; i < _buff[l].size(); i++) {
			line.push_back(TTF_RenderGlyph_Blended(font, _buff[l][i], {col[_hl[l][i]][0], col[_hl[l][i]][1], col[_hl[l][i]][2]}));
		}

		_map.push_back(line);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canv->w, canv->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, canv->pixels);
	for (int l = 0; l < _buff.size(); l++) {
		for (int i = 0; i < _buff[l].size(); i++) {
			if (_hl[l][i]) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * state::sz[0], l * state::sz[1], state::sz[0], state::sz[1], GL_BGRA, GL_UNSIGNED_BYTE, _map[l][i]->pixels);
			} else {
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * state::sz[0], l * state::sz[1], state::sz[0], state::sz[1], GL_BGRA, GL_UNSIGNED_BYTE, _map[l][i]->pixels);
			}
		}
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Console::push(char c) {
	if (!_buff.empty()) {
		_buff.back().push_back(c);
	} else {
		_buff.push_back({});
		_buff.back().push_back(c);
	}

	render();
}

void Console::newline() {
	_buff.push_back({});
}

void Console::pop() {
	if (!_buff.empty()) {
		if (!_buff.back().empty()) {
			_buff.back().pop_back();
		} else {
			_buff.pop_back();
		}
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
