#include <vector>
#include <Python.h>

#include <SDL2/SDL_ttf.h>

#include "console.h"
#include "col.h"
#include "state.h"

Console::Console(std::vector<std::string> buff) :
	Mesh(util::mesh::plane(glm::vec2(abs(-1 - 1), -1 -1)), "text", "text", glm::vec2(-1.0, 1.0)) {
		// text
		TTF_Init();

		TTF_Font* font = TTF_OpenFont("res/terminus.bdf", state::sz[1]);

		SDL_Surface* bg = SDL_CreateRGBSurface(0, bgRect.w, bgRect.h, 4 * sizeof (long int), 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_FillRect(bg, &bgRect, SDL_MapRGBA(bg->format, colS[false].r, colS[false].g, colS[false].b, 255));

		for (int l = 0; l < buff.size(); l++) {
			std::vector<bool> line;

			for (int i = 0; i < buff[l].size(); i++) {
				line.push_back(i > 2);
			}

			_active.push_back(line);
		}

		std::vector<std::vector<SDL_Surface*>> map;
		for (int l = 0; l < buff.size(); l++) {
			std::vector<SDL_Surface*> line;

			for (int i = 0; i < buff[l].size(); i++) {
				line.push_back(TTF_RenderGlyph_Blended(
					font,
					buff[l][i],
					colS[_active[l][i]]
				));
			}

			map.push_back(line);
		}

		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bg->w, bg->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, bg->pixels);
		for (int l = 0; l < buff.size(); l++) {
			for (int i = 0; i < buff[l].size(); i++) {
				if (_active[l][i]) {
					SDL_Surface* bg = SDL_CreateRGBSurface(0, bgRect.w, bgRect.h, 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
					SDL_FillRect(bg, &bgRect, SDL_MapRGBA(bg->format, colS[true].r, colS[true].g, colS[true].b, 255));

					glTexSubImage2D(GL_TEXTURE_2D, 0, i * charRect.w, l * charRect.h, charRect.w, charRect.h, GL_BGRA, GL_UNSIGNED_BYTE, bg->pixels);

					glTexSubImage2D(GL_TEXTURE_2D, 0, i * charRect.w, l * charRect.h, charRect.w, charRect.h, GL_BGRA, GL_UNSIGNED_BYTE, map[l][i]->pixels);
				} else {
					glTexSubImage2D(GL_TEXTURE_2D, 0, i * charRect.w, l * charRect.h, charRect.w, charRect.h, GL_BGRA, GL_UNSIGNED_BYTE, map[l][i]->pixels);
				}
			}
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		// Python
		Py_Initialize();
	}

void Console::draw() {
	glBindVertexArray(_id[VAO]);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, _pos.size());

	_prog.unUse();
	glBindVertexArray(0);
}
