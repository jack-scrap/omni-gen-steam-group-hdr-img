#include <thread>
#include <glm/gtc/type_ptr.hpp>

#include "disp.h"
#include "math.h"
#include "scn.h"
#include "line.h"
#include "mesh.h"
#include "omni.h"
#include "layout.h"

Disp::Disp(const char* title, glm::vec2 res, glm::vec3 bg) :
	t(0) {
		for (int i = 0; i < 2; i++) {
			_res[i] = res[i];
		}

		SDL_Init(SDL_INIT_VIDEO);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		_win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _res[X], _res[Y], SDL_WINDOW_OPENGL);

		_ctx = SDL_GL_CreateContext(_win);

		GLenum status = glewInit();
		if (status != GLEW_OK) {
			std::cerr << "Error: GLEW failed to initialize" << std::endl;
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		rend = SDL_CreateRenderer(
			_win,
			-1,
			0
			/* SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE */
			/* SDL_RENDERER_SOFTWARE */
			/* SDL_RENDERER_TARGETTEXTURE */
			/* SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE */
			/* SDL_RENDERER_PRESENTVSYNC */
			/* SDL_RENDERER_TARGETTEXTURE */
		);

		glClearColor(bg[R] / 255.0, bg[G] / 255.0, bg[B] / 255.0, 1.0);

		open = true;
	}

void Disp::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Disp::update() {
	SDL_GL_SwapWindow(_win);

	t = SDL_GetTicks();
}

Disp::~Disp() {
	SDL_GL_DeleteContext(_ctx);

	SDL_DestroyWindow(_win);

	SDL_Quit();
}
