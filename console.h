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

		TTF_Font* font;

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

		bool _w;

	public:
		unsigned int
			_mode,
			_sel,

			_curs[2][2],
			_idx[2][2],
			_l = 0;

		std::string _cwd;

		std::vector<std::map<std::string, std::string>> _tree;

		std::vector<std::string> _buff;
		std::string _name;

		std::string _prompt;

		char* _scr;
		bool* _hl;

		unsigned int
			_maxFs = 0,
			_maxNo = 0;

		const std::string _ps1 = "[] ";

		enum mode {
			FS,
			EDITOR,
			PROMPT
		};

		enum sel {
			IDX,
			LINE
		};

		enum id {
			VAO,

			VBO,

			STBO
		};

		enum attr {
			POS,
			ST
		};

		Console(std::string cwd, std::string name, std::vector<std::string> buff);

		void render();

		void push(char c);

		void enter();

		void newline();

		void exec();

		void pop();

		void draw();
};
