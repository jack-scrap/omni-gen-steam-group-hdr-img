#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "scn.h"
#include "util.h"
#include "state.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "omni.h"
#include "col.h"
#include "stb_image.h"

Disp* disp;
Console* console;

Cam cam = {
	{
		1000.0, 1000.0, 1000.0
	}, {
		50, 50, 50
	}
};

int main(int argc, char** argv) {
	bool boot;

	// initialize
	std::map<std::string, std::string> setting = util::cfg::parse<std::string>("player/cfg/init.cfg");

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

	std::string stage;
	unsigned int lvl;
	if (argv[1]) {
		omni::assertion(omni::stage.find(argv[1]) != omni::stage.end(), "Could not initialize; no corresponding section `" + std::string(argv[1]) + "`");

		stage = argv[1];
	} else {
		stage = "init";
	}
	if (argv[2]) {
		std::string name = argv[2];

		std::string path = std::string("lvl/") + argv[1] + std::string("/") + name + std::string(".json");
		std::ifstream in(path);
		omni::assertion(in.good(), "Could not initialize; no level `" + std::to_string(std::stoi(argv[2])) + std::string("` in stage `") + argv[1] + std::string("`"));

		lvl = atoi(argv[2]);
	} else {
		lvl = 0;
	}

	std::string name;
	if (argc > 1) {
		name = "player/script/" + stage + "/" + std::to_string(lvl) + "/main.py";
	} else {
		name = "player/doc/intro.txt";
	}

	console = new Console("player", name, util::fs::rd<std::vector<std::string>>(name));
	scn::init(stage, lvl);

	if (boot) {
		Obj* obj = objMk("truck/front", "obj", "solid", true);

		util::tex::spray(obj);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		GLfloat vtc[] = {
			-1.0, -1.0,
			1.0, -1.0,
			1.0, 1.0,
			-1.0, 1.0
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

		GLuint stbo;
		glGenBuffers(1, &stbo);
		glBindBuffer(GL_ARRAY_BUFFER, stbo);

		GLfloat st[] = {
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof st, st, GL_STATIC_DRAW);

		// shader
		Prog prog("logo", "tex");

		/// attribute
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		GLint attrPos = glGetAttribLocation(prog._id, "pos");
		glVertexAttribPointer(attrPos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(attrPos);

		glBindBuffer(GL_ARRAY_BUFFER, stbo);
		GLint attrSt = glGetAttribLocation(prog._id, "st");
		glVertexAttribPointer(attrSt, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(attrSt);

		/// uniform
		GLint uniActive = glGetUniformLocation(prog._id, "active");
		GLint uniRes = glGetUniformLocation(prog._id, "res");

		// initialize
		prog.use();

		glUniform1ui(uniActive, true);
		glUniform2i(uniRes, disp->_res[X], disp->_res[Y]);

		// texture
		int
			wd,
			ht,
			chan;
		GLubyte* data = stbi_load("res/dirt.jpg", &wd, &ht, &chan, 0);

		GLuint tex;
		if (data) {
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wd, ht, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			omni::err("Texture failed to load");
		}

		prog.unUse();

		// draw
		disp->clear();

		prog.use();

		glDrawArrays(GL_QUADS, 0, 2 + 2 * 3);

		prog.unUse();

		disp->update();

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	SDL_Event e;
	glm::vec3 prev;
	while (disp->_open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_F4:
						asdf._asdf++;

						break;

					case SDLK_F1:
						console->_mode = Console::FS;

						console->_idx[MIN][X] = 0;
						console->_idx[MIN][Y] = 1 + console->_l;
						console->_idx[MAX][X] = 0;
						console->_idx[MAX][Y] = 1 + console->_l;

						console->render();

						break;

					case SDLK_F2:
						console->_mode = Console::EDITOR;

						console->_idx[MIN][X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff.back().size();
						console->_idx[MIN][Y] = console->_buff.size();
						console->_idx[MAX][X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff.back().size();
						console->_idx[MAX][Y] = console->_buff.size();

						console->render();

						break;

					case SDLK_F3:
						console->_mode = Console::PROMPT;

						console->_idx[MIN][X] = (console->_ps1 + console->_prompt).size();
						console->_idx[MIN][Y] = state::line - 1;
						console->_idx[MAX][X] = (console->_ps1 + console->_prompt).size();
						console->_idx[MAX][Y] = state::line - 1;

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

						console->_idx[MIN][X] = 0;
						console->_idx[MIN][Y] = 1 + console->_l;
						console->_idx[MAX][X] = 0;
						console->_idx[MAX][Y] = 1 + console->_l;

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
								}

								// index
								if (console->_sel == Console::IDX) {
									switch (e.key.keysym.sym) {
										case SDLK_LEFT:
											if (console->_idx[MIN][X] > console->_maxFs + 1 + console->_maxNo + 1) {
												console->_idx[MIN][X]--;
												console->_idx[MAX][X]--;

												console->render();
											}

											break;

										case SDLK_RIGHT:
											if (console->_idx[MIN][X] < console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[MIN][Y] - 1].size()) {
												console->_idx[MIN][X]++;
												console->_idx[MAX][X]++;

												console->render();
											}

											break;

										case SDLK_DOWN:
											if (console->_idx[MIN][Y] < console->_buff.size()) {
												console->_idx[MIN][Y]++;
												console->_idx[MIN][X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[MIN][Y] - 1].size();
												console->_idx[MAX][Y]++;
												console->_idx[MAX][X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[MIN][Y] - 1].size();

												console->render();
											}

											break;

										case SDLK_UP:
											if (console->_idx[MIN][Y] > 1) {
												console->_idx[MIN][Y]--;
												console->_idx[MIN][X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[MIN][Y] - 1].size();
												console->_idx[MAX][Y]--;
												console->_idx[MAX][X] = console->_maxFs + 1 + console->_maxNo + 1 + console->_buff[console->_idx[MIN][Y] - 1].size();

												console->render();
											}

											break;
									}	
								}
							}
						}

						if (e.key.keysym.mod & (KMOD_ALT)) {
							console->_sel = Console::LINE;
						}

						if (e.key.keysym.sym == SDLK_ESCAPE) {
							console->_sel = Console::IDX;
						}

						break;
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				prev = cam._pos;

				SDL_GetMouseState(&cam._begin[0], &cam._begin[1]);

				cam._drag = true;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				cam._drag = false;

				prev = cam._pos;
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (cam._drag) {
					SDL_GetMouseState(&cam._curr[0], &cam._curr[1]);

					cam._delta[0] = cam._curr[0] - cam._begin[0];
					cam._delta[1] = cam._curr[1] - cam._begin[1];

					cam._pos[0] = prev[0] + cam._delta[0];
					cam._pos[2] = prev[0] - cam._delta[0];

					cam._pos[1] = prev[1] + cam._delta[1];
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
