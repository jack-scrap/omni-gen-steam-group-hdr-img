#include "disp.h"
#include "math.h"

Disp::Disp(const char* title, glm::vec2 res, glm::vec3 bg) :
	_res(res),
	_bg(bg) {
		SDL_Init(SDL_INIT_VIDEO);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _res[X], _res[Y], SDL_WINDOW_OPENGL);

		ctx = SDL_GL_CreateContext(win);

		GLenum status = glewInit();
		if (status != GLEW_OK) {
			std::cerr << "GLEW failed to initialize" << std::endl;
		}

		glClearColor(bg[R] / 255.0, bg[G] / 255.0, bg[B] / 255.0, 1);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		open = true;
	}

void Disp::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Disp::update() {
	SDL_GL_SwapWindow(win);
}

Disp::~Disp() {
	SDL_GL_DeleteContext(ctx);

	SDL_DestroyWindow(win);

	SDL_Quit();
}
