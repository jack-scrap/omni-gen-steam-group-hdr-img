#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Disp* disp;
Console* console;

Cam cam = {
	{
		-1000.0, 1000.0, -1000.0
	}, {
		50, 50, 50
	}, {
		-1000.0, 1000.0, -1000.0
	}
};

void spray(char c) {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	std::vector<GLfloat> vtc = util::mesh::rd::vtc(std::string("glyph/") + c);
	glBufferData(GL_ARRAY_BUFFER, vtc.size() * sizeof (GLfloat), &vtc[0], GL_STATIC_DRAW);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	std::vector<GLushort> idc = util::mesh::rd::idc(std::string("glyph/") + c);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idc.size() * sizeof (GLfloat), &idc[0], GL_STATIC_DRAW);

	// shader
	Prog prog("vec", "solid");

	prog.use();

	// matrix
	glm::mat4 model = glm::mat4(1.0);
	model = glm::rotate(model, (GLfloat) -(M_PI / 2), glm::vec3(1, 0, 0));

	/// attribute
	GLint attrPos = glGetAttribLocation(prog._id, "pos");
	glVertexAttribPointer(attrPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(attrPos);

	/// uniform
	GLint uniModel = glGetUniformLocation(prog._id, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	GLint uniActive = glGetUniformLocation(prog._id, "active");
	glUniform1ui(uniActive, true);

	prog.unUse();

	// framebuffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	
	/// color attachment
	GLuint cbo;
	glGenTextures(1, &cbo);
	glBindTexture(GL_TEXTURE_2D, cbo);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1000, 1000, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/// renderbuffer (stencil)
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 800, 600);

	/// attach texture, renderbuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error: Framebuffer not complete" << std::endl;
	}

	/// render
	prog.unUse();

	disp->clear();

	glBindVertexArray(vao);
	prog.use();

	glDrawElements(GL_TRIANGLES, idc.size(), GL_UNSIGNED_SHORT, (GLvoid*) 0);

	prog.unUse();
	glBindVertexArray(0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

int main(int argc, char** argv) {
	bool boot;

	// initialize
	std::map<std::string, std::string> setting = util::cfg::parse<std::string>("cfg/init.cfg");

	for (const auto& [key, val] : setting) {
		if (key == "skip_boot") {
			boot = !(val == "y");
		}

		if (key == "speed") {
			state::speed = std::stoi(val);
		}

		if (key == "wd") {
			layout::view[X] = std::stoi(val);
			layout::view[Y] = state::line * layout::dim[Y];
		}

		if (key == "fps") {
			state::fps = std::stoi(val);
		}

		if (key == "line_count") {
			state::line = std::stoi(val);
		}

		if (key == "line_width") {
			state::ln = std::stoi(val);
		}

		if (key == "tab_wd") {
			state::tabWd = std::stoi(val);
		}

		if (key == "time_format") {
			state::format = val;
		}
	}

	layout::res[X] = state::ln * layout::dim[X];
	layout::res[Y] = layout::view[Y];

	disp = new Disp("Omni", {
		layout::view[X] + (state::ln * layout::dim[X]), layout::view[Y]
	}, col[false]);

	unsigned int stage;
	unsigned int lvl;
	if (argv[1]) {
		stage = atoi(argv[1]);
	} else {
		stage = 0;
	}
	if (argv[2]) {
		lvl = atoi(argv[2]);
	} else {
		lvl = 0;
	}

	std::string name;
	if (argc > 1) {
		name = "script/" + omni::stage[stage] + "/" + std::to_string(lvl) + "/main.py";
	} else {
		name = "doc/intro.txt";
	}

	console = new Console(".", name, util::fs::rd<std::vector<std::string>>(name));
	scn::init(stage, lvl);

	if (boot) {
		/* GLuint vao; */
		/* glGenVertexArrays(1, &vao); */
		/* glBindVertexArray(vao); */

		/* GLuint vbo; */
		/* glGenBuffers(1, &vbo); */
		/* glBindBuffer(GL_ARRAY_BUFFER, vbo); */

		/* GLfloat vtc[] = { */
		/* 	-0.26, -0.26, */
		/* 	0.16, -0.26, */
		/* 	0.06, -0.10, */
		/* 	-0.10, -0.10, */

		/* 	0.16, -0.26, */
		/* 	0.26, -0.16, */
		/* 	0.10, -0.06, */
		/* 	0.06, -0.10, */

		/* 	0.26, -0.16, */
		/* 	0.26, 0.26, */
		/* 	0.10, 0.10, */
		/* 	0.10, -0.06, */

		/* 	0.26, 0.26, */
		/* 	-0.16, 0.26, */
		/* 	-0.06, 0.10, */
		/* 	0.10, 0.10, */

		/* 	-0.16, 0.26, */
		/* 	-0.26, 0.16, */
		/* 	-0.10, 0.06, */
		/* 	-0.06, 0.10, */

		/* 	-0.10, 0.06, */
		/* 	-0.26, 0.16, */
		/* 	-0.26, -0.26, */
		/* 	-0.10, -0.10 */
		/* }; */
		/* glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW); */

		/* // shader */
		/* Prog prog("logo", "solid"); */

		/* /// attribute */
		/* GLint attrPos = glGetAttribLocation(prog._id, "pos"); */
		/* glVertexAttribPointer(attrPos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0); */
		/* glEnableVertexAttribArray(attrPos); */

		/* /// uniform */
		/* GLint uniActive = glGetUniformLocation(prog._id, "active"); */
		/* GLint uniRes = glGetUniformLocation(prog._id, "res"); */

		/* // initialize */
		/* prog.use(); */

		/* glUniform1ui(uniActive, true); */
		/* glUniform2i(uniRes, disp->_res[X], disp->_res[Y]); */

		/* prog.unUse(); */

		/* // draw */
		/* disp->clear(); */

		/* prog.use(); */

		/* glDrawArrays(GL_QUADS, 0, (2 + 3) * 2 * 2 * 3); */

		/* prog.unUse(); */

		/* disp->update(); */

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		GLfloat vtc[] = {
			0.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,

			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

		// shader
		Prog prog("rend", "tex");

		GLuint stbo;
		glGenBuffers(1, &stbo);
		glBindBuffer(GL_ARRAY_BUFFER, stbo);

		GLfloat st[] = {
			0.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,

			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof st, st, GL_STATIC_DRAW);

		/// attribute
		GLint attrPos = glGetAttribLocation(prog._id, "pos");
		glVertexAttribPointer(attrPos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(attrPos);

		GLint attrSt = glGetAttribLocation(prog._id, "st");
		glVertexAttribPointer(attrSt, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(attrSt);

		/// uniform
		GLint uniActive = glGetUniformLocation(prog._id, "active");
		GLint uniRes = glGetUniformLocation(prog._id, "res");

		spray('0');

		// initialize
		prog.use();

		glUniform1ui(uniActive, true);
		glUniform2i(uniRes, disp->_res[X], disp->_res[Y]);

		prog.unUse();

		// draw
		disp->clear();

		glBindVertexArray(vao);
		prog.use();

		glDrawArrays(GL_TRIANGLES, 0, sizeof vtc / sizeof *vtc);

		prog.unUse();
		glBindVertexArray(0);

		disp->update();

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	SDL_Event e;
	while (disp->_open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_F1:
						console->_mode = Console::FS;

						console->_idx[X] = 0;
						console->_idx[Y] = 1 + console->_l;

						console->render();

						break;

					case SDLK_F2:
						console->_mode = Console::EDITOR;

						console->_idx[X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff.back().size();
						console->_idx[Y] = console->_buff.size();

						console->render();

						break;

					case SDLK_F3:
						console->_mode = Console::PROMPT;

						console->_idx[X] = (console->_ps1 + console->_prompt).size();
						console->_idx[Y] = state::line - 1;

						console->render();

						break;
				}

				switch (console->_mode) {
					case Console::FS:
						console->_modeStr = "FS";

						break;

					case Console::EDITOR:
						console->_modeStr = "EDITOR";

						break;

					case Console::PROMPT:
						console->_modeStr = "PROMPT";

						break;
				};

				switch (console->_mode) {
					case Console::FS:
						switch (e.key.keysym.sym) {
							case SDLK_DOWN:
								if (console->_l < console->_tree.size() - 1) {
									console->_l++;
								}

								break;

							case SDLK_UP:
								if (console->_l > 0) {
									console->_l--;
								}

								break;
						}

						console->_idx[X] = 0;
						console->_idx[Y] = 1 + console->_l;

						for (int l = 0; l < console->_tree.size(); l++) {
							for (int i = 0; i < console->_maxFs; i++) {
								console->_hl[((1 + l) * state::ln) + i] = false;
							}
						}

						for (int i = 0; i < console->_maxFs; i++) {
							console->_hl[((1 + console->_l) * state::ln) + i] = true;
						}

						console->render();

						break;

					case Console::EDITOR:
					case Console::PROMPT:
						// modifier
						if (e.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
							if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
								console->push((char) (e.key.keysym.sym - 32));
							} else if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
								char c;
								switch (e.key.keysym.sym) {
									case SDLK_1:
										c = '!';

										break;

									case SDLK_2:
										c = '@';

										break;

									case SDLK_3:
										c = '#';

										break;

									case SDLK_4:
										c = '$';

										break;

									case SDLK_5:
										c = '%';

										break;

									case SDLK_6:
										c = '^';

										break;

									case SDLK_7:
										c = '&';

										break;

									case SDLK_8:
										c = '*';

										break;

									case SDLK_9:
										c = '(';

										break;

									case SDLK_0:
										c = ')';

										break;
								}

								console->push((char) c);
							} else {
								switch (e.key.keysym.sym) {
									case SDLK_BACKQUOTE:
										console->push('~');

										break;

									case SDLK_MINUS:
										console->push('_');

										break;

									case SDLK_EQUALS:
										console->push('+');

										break;

										// bracket
									case SDLK_LEFTBRACKET:
										console->push('{');

										break;

									case SDLK_RIGHTBRACKET:
										console->push('}');

										break;

									case SDLK_BACKSLASH:
										console->push('|');

										break;

									case SDLK_SEMICOLON:
										console->push(':');

										break;

									case SDLK_QUOTE:
										console->push('"');

										break;

									case SDLK_COMMA:
										console->push('<');

										break;

									case SDLK_PERIOD:
										console->push('>');

										break;

									case SDLK_SLASH:
										console->push('?');

										break;
								}
							}
						} else {
							if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
								console->push((char) e.key.keysym.sym);
							} else if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
								console->push((char) e.key.keysym.sym);
							} else {
								switch (e.key.keysym.sym) {
									case SDLK_SPACE:
										console->push(' ');

										break;

									case SDLK_TAB:
										for (int i = 0; i < state::tabWd; i++) {
											console->push(' ');
										}

										break;

									case SDLK_BACKQUOTE:
										console->push('`');

										break;

									case SDLK_MINUS:
										console->push('-');

										break;

									case SDLK_EQUALS:
										console->push('=');

										break;

									// bracket
									case SDLK_LEFTBRACKET:
										console->push('[');

										break;

									case SDLK_RIGHTBRACKET:
										console->push(']');

										break;

									case SDLK_BACKSLASH:
										console->push('\\');

										break;

									case SDLK_SEMICOLON:
										console->push(';');

										break;

									case SDLK_QUOTE:
										console->push('\'');

										break;

									case SDLK_COMMA:
										console->push(',');

										break;

									case SDLK_PERIOD:
										console->push('.');

										break;

									case SDLK_SLASH:
										console->push('/');

										break;

									case SDLK_RETURN:
										console->enter();

										break;

									case SDLK_BACKSPACE:
										console->pop();

										break;

									// index
									case SDLK_LEFT:
										if (console->_idx[X] > console->_maxFs + 1 + console->_maxNo + 1) {
											console->_idx[X]--;

											console->render();
										}

										break;

									case SDLK_RIGHT:
										if (console->_idx[X] < console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[Y] - 1].size()) {
											console->_idx[X]++;

											console->render();
										}

										break;

									case SDLK_DOWN:
										if (console->_idx[Y] < console->_buff.size()) {
											console->_idx[Y]++;
											console->_idx[X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[Y] - 1].size();

											console->render();
										}

										break;

									case SDLK_UP:
										if (console->_idx[Y] > 1) {
											console->_idx[Y]--;
											console->_idx[X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[Y] - 1].size();

											console->render();
										}

										break;
								}
							}
						}

						break;
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				SDL_GetMouseState(&cam._begin[0], &cam._begin[1]);

				cam._drag = true;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				cam._drag = false;

				cam._prev = cam._pos;
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (cam._drag) {
					SDL_GetMouseState(&cam._curr[0], &cam._curr[1]);

					cam._delta[0] = cam._curr[0] - cam._begin[0];
					cam._delta[1] = cam._curr[1] - cam._begin[1];

					cam._pos[0] = cam._prev[0] + cam._delta[0];
					cam._pos[2] = cam._prev[0] - cam._delta[0];

					cam._pos[1] = cam._prev[1] + cam._delta[1];
				}
			}

			if (e.type == SDL_MOUSEWHEEL) {
				if (e.wheel.y > 0) {
					if (glm::all(glm::lessThan(cam._scale, glm::vec3(100.0, 100.0, 100.0)))) {
						cam._scale += glm::vec3(10.0, 10.0, 10.0);
					}
				}

				if (e.wheel.y < 0) {
					if (glm::all(glm::greaterThan(cam._scale, glm::vec3(20.0, 20.0, 20.0)))) {
						cam._scale -= glm::vec3(10.0, 10.0, 10.0);
					}
				}
			}

			if (e.type == SDL_QUIT) {
				disp->_open = false;
			}
		}

		disp->draw();
	}

	return 0;
}
