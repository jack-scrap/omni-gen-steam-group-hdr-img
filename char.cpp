#include <iostream>

#include "char.h"
#include "col.h"
#include "state.h"

Char::Char(char c, bool active, TTF_Font* font, glm::vec2 loc) :
	Mesh(util::mesh::plane(glm::vec2(state::step[0], state::step[1])), "text", "text", glm::vec2(-1.0, 1.0 - 0.1) + glm::vec2(loc[0] * state::step[0], -(loc[1] * state::step[1]))) {
		SDL_Color col = colS[active];

		SDL_Surface* surf = TTF_RenderGlyph_Blended(font, c, col);

		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

void Char::draw() {
	glBindVertexArray(_id[VAO]);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, _pos.size());

	_prog.unUse();
	glBindVertexArray(0);
}
