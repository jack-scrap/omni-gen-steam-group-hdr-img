#include "disp.h"

Disp::Disp(const char* title, int wd, int ht) :
	_wd(wd),
	_ht(ht) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wd, ht, SDL_WINDOW_OPENGL);

	ctx = SDL_GL_CreateContext(win);

	GLenum status = glewInit();
	if (status != GLEW_OK) {
		std::cerr << "GLEW failed to initialize" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(20);

	open = true;
}

void Disp::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1);
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
