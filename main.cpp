#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <Python.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glm/gtc/type_ptr.hpp>

#include "scn.h"
#include "util.h"
#include "state.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "omni.h"
#include "col.h"
#include "layout.h"
#include "class_def.h"

#define dim 184

typedef struct {
	unsigned int _r;
	unsigned int _g;
	unsigned int _b;
	unsigned int _a;
} Col;

Disp* disp;
Console* console;

Cam cam;

bool boot = false;

extern std::string stage;
extern unsigned int lvl;

uint32_t rmask = 0xff000000;
uint32_t gmask = 0x00ff0000;
uint32_t bmask = 0x0000ff00;
uint32_t amask = 0x000000ff;

bool scr(std::string filepath, SDL_Window* SDLWindow, SDL_Renderer* SDLRenderer) {
	SDL_Surface* saveSurface = NULL;
	SDL_Surface* infoSurface = NULL;
	infoSurface = SDL_GetWindowSurface(SDLWindow);
	if (infoSurface == NULL) {
		std::cerr << "Failed to create info surface from window in save(string), SDL_GetError() - " << SDL_GetError() << "\n";
	} else {
		unsigned char* pixels = new (std::nothrow) unsigned char[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
		if (!pixels) {
			std::cerr << "Unable to allocate memory for screenshot pixel data buffer!\n";

			return false;
		} else {
			if (SDL_RenderReadPixels(SDLRenderer, &infoSurface->clip_rect, infoSurface->format->format, pixels, infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
				std::cerr << "Failed to read pixel data from SDL_Renderer object. SDL_GetError() - " << SDL_GetError() << "\n";

				delete[] pixels;

				return false;
			} else {
				saveSurface = SDL_CreateRGBSurfaceFrom(
					pixels,
					infoSurface->w,
					infoSurface->h,
					infoSurface->format->BitsPerPixel,
					infoSurface->w * infoSurface->format->BytesPerPixel,
					infoSurface->format->Rmask,
					infoSurface->format->Gmask,
					infoSurface->format->Bmask,
					infoSurface->format->Amask
				);

				if (saveSurface == NULL) {
					std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";

					delete[] pixels;

					return false;
				}

				SDL_SaveBMP(saveSurface, filepath.c_str());
				SDL_FreeSurface(saveSurface);
				saveSurface = NULL;
			}

			delete[] pixels;
		}

		SDL_FreeSurface(infoSurface);
		infoSurface = NULL;
	}

	return true;
}

int main(int argc, char* argv[]) {
	disp = new Disp("Omni", {
		dim,
		dim
	}, {
		38, 38, 38
	});

	GLfloat vtc[] = {
		-0.26, 0.0, -0.26,
		0.16, 0.0, -0.26,
		-0.10, 0.0, -0.10,
		0.06, 0.0, -0.10,

		0.16, 0.0, -0.26,
		0.26, 0.0, -0.16,
		0.06, 0.0, -0.10,
		0.10, 0.0, -0.06,

		0.26, 0.0, -0.16,
		0.26, 0.0, 0.26,
		0.10, 0.0, -0.06,
		0.10, 0.0, 0.10,

		0.26, 0.0, 0.26,
		-0.16, 0.0, 0.26,
		0.10, 0.0, 0.10,
		-0.06, 0.0, 0.10,

		-0.16, 0.0, 0.26,
		-0.26, 0.0, 0.16,
		-0.06, 0.0, 0.10,
		-0.10, 0.0, 0.06,

		-0.10, 0.0, 0.06,
		-0.26, 0.0, 0.16,
		-0.10, 0.0, -0.10,
		-0.26, 0.0, -0.26
	};

	GLushort idc[] = {
		0, 1, 2,
		2, 1, 3,

		4, 5, 6,
		6, 5, 7,

		8, 9, 10,
		10, 9, 11,

		12, 13, 14,
		14, 13, 15,

		16, 17, 18,
		18, 17, 19,

		20, 21, 22,
		22, 21, 23
	};

	Mesh* mesh = meshMk(vtc, idc, sizeof idc / sizeof *idc);

	Prog prog("logo", "solid");

	prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, mesh->_id[Mesh::VBO]);
	GLint attrPos = glGetAttribLocation(prog._id, "pos");
	glVertexAttribPointer(attrPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(attrPos);

	// uniform
	GLint uniRes = glGetUniformLocation(prog._id, "res");
	GLint uniActive = glGetUniformLocation(prog._id, "active");

	glUniform2ui(uniRes, disp->_res[X], disp->_res[Y]);

	glUniform1ui(uniActive, true);

	prog.unUse();

	// draw
	disp->clear();

	glBindVertexArray(mesh->_id[Mesh::VAO]);
	prog.use();

	glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	prog.unUse();
	glBindVertexArray(0);

	meshDel(mesh);

	disp->update();

	SDL_RenderPresent(disp->rend);

	SDL_Surface* surf = SDL_CreateRGBSurface(0, dim, dim, 32, 0, 0, 0, 0);
	SDL_RenderReadPixels(disp->rend, NULL, surf->format->format, surf->pixels, surf->pitch);

	IMG_SavePNG(surf, "o/scr.png");

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	return 0;
}
