#include <thread>
#include <glm/gtc/type_ptr.hpp>

#include "disp.h"
#include "math.h"
#include "scn.h"
#include "line.h"

Disp::Disp(const char* title, glm::vec2 res, glm::vec3 bg) {
	for (int i = 0; i < 2; i++) {
		_res[i] = res[i];
	}

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

	for (Obj* _ : mesh) {
		objDraw(_);
	}

	for (Obj* _ : line) {
		lineDraw(_);
	}

	for (Obj* _ : pt) {
		ptDraw(_);
	}

	glViewport(0, 0, layout::res[Y], layout::view[Y]);

	console->draw();

	update();
}

Disp::~Disp() {
	SDL_GL_DeleteContext(_ctx);

	SDL_DestroyWindow(_win);

	SDL_Quit();
}
