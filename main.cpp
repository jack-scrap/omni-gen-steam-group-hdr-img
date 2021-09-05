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
#include "layout.h"
#include "stb_image.h"

Disp* disp;
Console* console;

Cam cam;

bool boot = false;

int main(int argc, char** argv) {
	cam._pos = {
		1000.0, 1000.0, 1000.0
	};
	cam._scale = {
		50, 50, 50
	};

	// initialize
	unsigned int res[2];

	std::map<std::string, std::string> setting = util::cfg::parse<std::string>("player/cfg/init.cfg");

	for (const auto& [key, val] : setting) {
		if (key == "line_count") {
			res[Y] = std::stoi(val);
		}

		if (key == "line_width") {
			res[X] = std::stoi(val);
		}

		if (key == "begin_index_at_one") {
			state::initIdx = val == "y";
		}

		if (key == "skip_boot") {
			boot = !(val == "y");
		}

		if (key == "speed") {
			state::speed = std::stoi(val);
		}

		if (key == "wd") {
			layout::view[X] = std::stoi(val);
			layout::view[Y] = state::line * layout::glyph[Y];
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

	layout::res[X] = state::ln * layout::glyph[X];
	layout::res[Y] = layout::view[Y];

	for (int i = 0; i < 2; i++) {
		layout::canv[i] = res[i] * layout::glyph[i];
	}

	disp = new Disp("Omni", {
		layout::view[X] + (state::ln * layout::glyph[X]), layout::view[Y]
	}, col[false]);

	std::string stage;
	unsigned int lvl;
	if (argc > 1) {
		omni::assertion(omni::stage.find(argv[1]) != omni::stage.end(), "Could not initialize; no corresponding section `" + std::string(argv[1]) + "`");

		stage = argv[1];
	} else {
		stage = "init";
	}
	if (argc > 2) {
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

	console = new Console("player/script/init/0/main.py", "player", res);
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
				if (!SDL_GetModState()) {
					// text
					if (
						e.key.keysym.sym >= SDLK_a &&
						e.key.keysym.sym <= SDLK_z
					) {
						console->ins(e.key.keysym.sym);
					}

					if ((
						e.key.keysym.sym >= SDLK_0 &&
						e.key.keysym.sym <= SDLK_9
					)) {
						console->ins(e.key.keysym.sym);
					}

					if ((e.key.keysym.sym == SDLK_SPACE) || (
						e.key.keysym.sym == SDLK_COMMA ||
						e.key.keysym.sym == SDLK_PERIOD ||
						e.key.keysym.sym == SDLK_SLASH ||

						e.key.keysym.sym == SDLK_SEMICOLON ||
						e.key.keysym.sym == SDLK_QUOTE ||

						e.key.keysym.sym == SDLK_LEFTBRACKET ||
						e.key.keysym.sym == SDLK_RIGHTBRACKET ||
						e.key.keysym.sym == SDLK_BACKSLASH ||

						e.key.keysym.sym == SDLK_MINUS ||
						e.key.keysym.sym == SDLK_EQUALS ||

						e.key.keysym.sym == SDLK_BACKQUOTE
					)) {
						console->ins(e.key.keysym.sym);
					}

					// control
					if (e.key.keysym.sym == SDLK_BACKSPACE) {
						console->del();
					}

					if (e.key.keysym.sym == SDLK_RETURN) {
						switch (console->_mode) {
							case Console::EDITOR:
								console->newLine();

								break;

							case Console::PROMPT:
								console->exec();

								break;

							case Console::FS:
								if (console->_tree[console->_cursFs]["type"] == "file") {
									console->open(console->_cwd + "/" + console->_tree[console->_cursFs]["name"]);
								}

								if (console->_tree[console->_cursFs]["type"] == "dir") {
									console->changeDir(console->_cwd + "/" + console->_tree[console->_cursFs]["name"]);
								}

								break;
						}
					}

					// mode
					if (e.key.keysym.sym == SDLK_F1) {
						console->switchMode(Console::EDITOR);
					}

					if (e.key.keysym.sym == SDLK_F2) {
						console->switchMode(Console::PROMPT);
					}

					if (e.key.keysym.sym == SDLK_F3) {
						console->switchMode(Console::FS);
					}

					if (e.key.keysym.sym == SDLK_ESCAPE) {
						switch (console->_mode) {
							case Console::EDITOR:
								console->_rngEditor = false;

								break;

							case Console::PROMPT:
								console->_rngPrompt = false;

								break;
						}
					}

					// navigation
					switch (console->_mode) {
						case Console::EDITOR:
							if (e.key.keysym.sym == SDLK_RIGHT) {
								if (console->_cursEditor[console->_rngEditor][X] < console->_buff[console->_cursEditor[console->_rngEditor][Y]].size()) {
									console->_cursEditor[console->_rngEditor][X]++;
								}
							}

							if (e.key.keysym.sym == SDLK_LEFT) {
								if (console->_cursEditor[console->_rngEditor][X] > 0) {
									console->_cursEditor[console->_rngEditor][X]--;
								}
							}

							if (e.key.keysym.sym == SDLK_DOWN) {
								if (console->_cursEditor[console->_rngEditor][X] == console->_buff[console->_cursEditor[console->_rngEditor][Y]].size()) {
									if (console->_cursEditor[console->_rngEditor][Y] < console->_buff.size() - 1) {
										console->_cursEditor[console->_rngEditor][Y]++;

										console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
									}
								} else {
									if (console->_cursEditor[console->_rngEditor][Y] < console->_buff.size() - 1) {
										console->_cursEditor[console->_rngEditor][Y]++;

										if (console->_cursEditor[console->_rngEditor][X] > console->_buff[console->_cursEditor[console->_rngEditor][Y]].size()) {
											console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
										}
									}
								}

							}

							if (e.key.keysym.sym == SDLK_UP) {
								if (console->_cursEditor[console->_rngEditor][X] == console->_buff[console->_cursEditor[console->_rngEditor][Y]].size()) {
									if (console->_cursEditor[console->_rngEditor][Y] > 0) {
										console->_cursEditor[console->_rngEditor][Y]--;

										console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
									}
								} else {
									if (console->_cursEditor[console->_rngEditor][Y] > 0) {
										console->_cursEditor[console->_rngEditor][Y]--;

										if (console->_cursEditor[console->_rngEditor][X] > console->_buff[console->_cursEditor[console->_rngEditor][Y]].size()) {
											console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
										}
									}
								}
							}

							break;

						case Console::PROMPT:
							if (e.key.keysym.sym == SDLK_RIGHT) {
								if (console->_cursPrompt[console->_rngPrompt] < console->_prompt.size()) {
									console->_cursPrompt[console->_rngPrompt]++;
								}
							}

							if (e.key.keysym.sym == SDLK_LEFT) {
								if (console->_cursPrompt[console->_rngPrompt] > 0) {
									console->_cursPrompt[console->_rngPrompt]--;
								}
							}

							break;

						case Console::FS:
							if (e.key.keysym.sym == SDLK_DOWN) {
								if (console->_cursFs < console->_tree.size() - 1) {
									console->_cursFs++;
								}
							}

							if (e.key.keysym.sym == SDLK_UP) {
								if (console->_cursFs > 0) {
									console->_cursFs--;
								}
							}

							break;
					}
				} else {
					if (e.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
						// text
						if (
							e.key.keysym.sym >= SDLK_a &&
							e.key.keysym.sym <= SDLK_z
						) {
							char c = e.key.keysym.sym;

							c -= 32;

							console->ins(c);
						}

						if (
							e.key.keysym.sym >= SDLK_0 &&
							e.key.keysym.sym <= SDLK_9
						) {
							char c;
							switch (e.key.keysym.sym) {
								case SDLK_0:
									c = ')';

									break;

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
							}

							console->ins(c);
						}

						if ((e.key.keysym.sym == SDLK_SPACE) || (
							e.key.keysym.sym == SDLK_COMMA ||
							e.key.keysym.sym == SDLK_PERIOD ||
							e.key.keysym.sym == SDLK_SLASH ||

							e.key.keysym.sym == SDLK_SEMICOLON ||
							e.key.keysym.sym == SDLK_QUOTE ||

							e.key.keysym.sym == SDLK_LEFTBRACKET ||
							e.key.keysym.sym == SDLK_RIGHTBRACKET ||
							e.key.keysym.sym == SDLK_BACKSLASH ||

							e.key.keysym.sym == SDLK_MINUS ||
							e.key.keysym.sym == SDLK_EQUALS ||

							e.key.keysym.sym == SDLK_BACKQUOTE
						)) {
							char c;
							switch (e.key.keysym.sym) {
								case SDLK_BACKQUOTE:
									c = '~';

									break;

								case SDLK_COMMA:
									c = '<';

									break;

								case SDLK_PERIOD:
									c = '>';

									break;

								case SDLK_SLASH:
									c = '?';

									break;

								case SDLK_SEMICOLON:
									c = ':';

									break;

								case SDLK_QUOTE:
									c = '"';

									break;

								case SDLK_LEFTBRACKET:
									c = '{';

									break;

								case SDLK_RIGHTBRACKET:
									c = '}';

									break;

								case SDLK_BACKSLASH:
									c = '|';

									break;

								case SDLK_MINUS:
									c = '_';

									break;

								case SDLK_EQUALS:
									c = '+';

									break;
							}

							console->ins(c);
						}
					}

					if (e.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
						switch (console->_mode) {
							case Console::EDITOR:
								console->_rngEditor = true;

								break;

							case Console::PROMPT:
								console->_rngPrompt = true;

								break;
						}
					}

					if (e.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
						switch (e.key.keysym.sym) {
							case SDLK_c:
								switch (console->_mode) {
									case Console::EDITOR: {
										console->_clip.clear();

										unsigned int idx[2];
										for (int i = 0; i < 2; i++) {
											idx[i] = console->_cursEditor[MIN][i];
										}
										while (idx[Y] < console->_cursEditor[MAX][Y]) {
											console->_clip.push_back(std::string(console->_buff[idx[Y]].begin() + idx[X], console->_buff[idx[Y]].end()));

											idx[Y]++;
											idx[X] = 0;
										}

										console->_clip.push_back(std::string(console->_buff[idx[Y]].begin(), console->_buff[idx[Y]].begin() + console->_cursEditor[MAX][X] + 1));

										break;
									}

									case Console::PROMPT:
										console->_clip = {
											std::string(console->_prompt.begin() + console->_cursPrompt[MIN], console->_prompt.begin() + console->_cursPrompt[MAX] + 1)
										};

										break;
								}

								break;

							case SDLK_v:
								switch (console->_mode) {
									case Console::EDITOR:
										console->del();

										console->_buff[console->_cursEditor[console->_rngEditor][Y]].insert(console->_cursEditor[MIN][X], console->_clip[0]);

										break;

									case Console::PROMPT:
										console->del();

										console->_prompt.insert(console->_cursPrompt[MIN], console->_clip[0]);

										break;
								}

								break;

							case SDLK_x:
								switch (console->_mode) {
									case Console::EDITOR:
										console->_clip = {
											std::string(console->_buff[console->_cursEditor[Y][console->_rngEditor]].begin() + console->_cursEditor[MIN][X], console->_buff[console->_cursEditor[Y][console->_rngEditor]].begin() + console->_cursEditor[MAX][X] + 1)
										};

										console->del();

										break;

									case Console::PROMPT:
										console->_clip = {
											std::string(console->_prompt.begin() + console->_cursPrompt[MIN], console->_prompt.begin() + console->_cursPrompt[MAX] + 1)
										};

										console->del();

										break;
								}

								break;
						}
					}
				}

				if (!console->_rngEditor) {
					for (int r = 0; r < 2; r++) {
						console->_cursEditor[MAX][r] = console->_cursEditor[MIN][r];
					}
				}

				if (!console->_rngPrompt) {
					console->_cursPrompt[MAX] = console->_cursPrompt[MIN];
				}

				console->clear();

				console->fmt();
				console->hl();

				console->render();
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
