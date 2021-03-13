#pragma once

#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>

#include "layout.h"
#include "state.h"
#include "math.h"
#include "util.h"

class Console {
	private:
		std::vector<GLfloat> _st = util::mesh::plane(glm::vec2(1, 1));

		GLuint _id[3];

		GLint _attr[2];

		Prog _prog;


		const std::string _ps1 = "[] ";

		TTF_Font* font;

		std::vector<std::vector<bool>> _hl;

		SDL_Surface
			* _canv = SDL_CreateRGBSurface(0, layout::res[X], layout::res[Y], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000),
			* _block = SDL_CreateRGBSurface(0, layout::dim[X], layout::dim[Y], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

		SDL_Rect
			_canvRect = {
				0,
				0,
				state::ln * layout::dim[X],
				layout::view[Y]
			},

			_blockRect = {
				0,
				0,
				layout::dim[X],
				layout::dim[Y]
			};

		std::vector<std::vector<SDL_Surface*>>
			_map,
			_no;

		GLuint _tex;

	public:
		enum mode {
			EDITOR,
			CMD
		};

		unsigned int
			_mode,

			_idx[2];

		std::vector<std::string> _buff;
		std::string _name;

		std::string _prompt;

		Console(std::string name, std::vector<std::string> buff);

		void render();

		void push(char c);

		void enter();

		void newline();

		void exec();

		void save(std::string name);

		void pop();

		void draw();
};
