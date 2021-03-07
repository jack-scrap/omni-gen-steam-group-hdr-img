#include "disp.h"
#include "math.h"
#include "scn.h"

Disp::Disp(const char* title, glm::vec2 res, glm::vec3 bg) :
	_res(res) {
		SDL_Init(SDL_INIT_VIDEO);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		_win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _res[X], _res[Y], SDL_WINDOW_OPENGL);

		_ctx = SDL_GL_CreateContext(_win);

		GLenum status = glewInit();
		if (status != GLEW_OK) {
			std::cerr << "Error: GLEW failed to initialize" << std::endl;
		}

		glClearColor(bg[R] / 255.0, bg[G] / 255.0, bg[B] / 255.0, 1);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		_open = true;
	}

void Disp::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Disp::update() {
	SDL_GL_SwapWindow(_win);
}

void Disp::draw() {
	clear();

	glEnable(GL_DEPTH_TEST);

	glViewport(layout::res[X], 0, layout::view[X], layout::view[Y]);

	for (Obj* _ : scn) {
		objDraw(_);
	}

	glViewport(0, 0, layout::res[Y], layout::view[Y]);

	console->draw();

	update();

	char lhs[data->_x];
	for (int i = 0; i < data->_x; i++) {
		if (data->_data[i]->_data) {
			lhs[i] = data->_data[i]->_data->_c;
		} else {
			lhs[i] = 0;
		}
	}

/* 	for (int i = 0; i < data->_x; i++) { */
/* 		std::cout << lhs[i] << std::endl; */
/* 	} */
/* 	std::cout << std::endl; */

	eq = true;
	for (int i = 0; i < data->_x; i++) {
		if (lhs[i] != rhs[i]) {
			eq = false;
		}
	}

	/* std::cout << eq << std::endl; */
}

Disp::~Disp() {
	SDL_GL_DeleteContext(_ctx);

	SDL_DestroyWindow(_win);

	SDL_Quit();
}
