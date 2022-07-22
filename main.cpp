#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <SDL2/SDL.h>
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

Disp* disp;
Console* console;

Cam cam;

bool boot = false;

extern std::string stage;
extern unsigned int lvl;

void handle() {
	SDL_Event e;
	while (SDL_WaitEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			if (!SDL_GetModState()) {
				if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
					console->ins(e.key.keysym.sym);
				}

				if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
					console->ins(e.key.keysym.sym);
				}

				if (
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
				) {
					console->ins(e.key.keysym.sym);
				}

				// whitespace
				if (e.key.keysym.sym == SDLK_SPACE) {
					console->ins(e.key.keysym.sym);
				}

				if (e.key.keysym.sym == SDLK_TAB) {
					if (state::expandTab) {
						for (int i = 0; i < state::tabWd; i++) {
							console->ins(' ');
						}
					} else {
						console->ins('\t');
					}
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
								console->open(console->_cwd + util::fs::path::sep + console->_tree[console->_cursFs]["name"]);
							}

							if (console->_tree[console->_cursFs]["type"] == "dir") {
								console->changeDir(console->_cwd + util::fs::path::sep + console->_tree[console->_cursFs]["name"]);
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
							if (console->_cursEditor[console->_rngEditor][Y] < console->_buff.size() - 1) {
								bool trail = console->_cursEditor[console->_rngEditor][X] == console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();

								console->_cursEditor[console->_rngEditor][Y]++;

								// pin to end
								if (trail) {
									console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
								}
							}

							console->_cursEditor[console->_rngEditor][X] = util::math::clamp<unsigned int>(console->_cursEditor[console->_rngEditor][X], 1, console->_buff[console->_cursEditor[console->_rngEditor][Y]].size());
						}

						if (e.key.keysym.sym == SDLK_UP) {
							if (console->_cursEditor[console->_rngEditor][Y] > 0) {
								bool trail = console->_cursEditor[console->_rngEditor][X] == console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();

								console->_cursEditor[console->_rngEditor][Y]--;

								// pin to end
								if (trail) {
									console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
								}
							}

							console->_cursEditor[console->_rngEditor][X] = util::math::clamp<unsigned int>(console->_cursEditor[console->_rngEditor][X], 1, console->_buff[console->_cursEditor[console->_rngEditor][Y]].size());
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
							if (console->_cursFs) {
								console->_cursFs--;
							}
						}

						break;
				}
			} else {
				if (e.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
					// text
					if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
						char c = e.key.keysym.sym;

						c -= 32;

						console->ins(c);
					}

					if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
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

					if (
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
					) {
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
						// clipboard
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
								case Console::EDITOR: {
									console->_clip = Console::slice(console->_buff, console->_cursEditor);

									console->del();

									break;
								}

								case Console::PROMPT:
									console->_clip = {
										std::string(console->_prompt.begin() + console->_cursPrompt[MIN], console->_prompt.begin() + console->_cursPrompt[MAX] + 1)
									};

									console->del();

									break;
							}

							break;

						// history
						case SDLK_z:
							if (console->_h) {
								console->_h--;

								console->_buff = console->_hist[console->_h];

								for (int r = 0; r < 2; r++) {
									if (console->_cursEditor[r][Y] > console->_buff.size()) {
										console->_cursEditor[r][Y]--;
									}

									if (console->_cursEditor[r][X] > console->_buff[console->_cursEditor[r][Y]].size()) {
										console->_cursEditor[r][X] = console->_buff[console->_cursEditor[r][Y]].size();
									}
								}
							}

							break;

						case SDLK_r:
							if (console->_h < console->_hist.size() - 1) {
								console->_h++;

								console->_buff = console->_hist[console->_h];
							}

							for (int r = 0; r < 2; r++) {
								if (console->_cursEditor[r][Y] > console->_buff.size()) {
									console->_cursEditor[r][Y]--;
								}

								if (console->_cursEditor[r][X] > console->_buff[console->_cursEditor[r][Y]].size()) {
									console->_cursEditor[r][X] = console->_buff[console->_cursEditor[r][Y]].size();
								}
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
		}

		if (e.type == SDL_MOUSEBUTTONDOWN) {
			SDL_GetMouseState(&cam._begin[0], &cam._begin[1]);

			if (
				cam._begin[X] > layout::canv[X] &&
				cam._begin[Y] > 0 &&
				cam._begin[X] < layout::canv[X] + layout::view[X] &&
				cam._begin[Y] < layout::view[Y]
			) {
				cam._posPrev = cam._pos;

				cam._drag = true;
			}
		}

		if (e.type == SDL_MOUSEBUTTONUP) {
			cam._pos = glm::clamp(cam._posPrev + cam._posDelta, glm::vec3(10000.0 - 500.0), glm::vec3(10000.0 + 500.0));

			cam._drag = false;
		}

		if (e.type == SDL_MOUSEMOTION) {
			if (cam._drag) {
				SDL_GetMouseState(&cam._curr[0], &cam._curr[1]);

				for (int i = 0; i < 2; i++) {
					cam._delta[i] = cam._curr[i] - cam._begin[i];
				}

				cam._posDelta[X] = cam._delta[X];
				cam._posDelta[Z] = -cam._delta[X];

				cam._posDelta[Y] = -cam._delta[Y];

				cam._pos = glm::clamp(cam._posPrev + cam._posDelta, glm::vec3(10000.0 - 500.0), glm::vec3(10000.0 + 500.0));
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
			disp->open = false;
		}
	}
}

int main(int argc, char* argv[]) {
	// initialize
	std::vector<std::map<std::string, std::string>> dir = util::fs::ls("player/cfg");

	for (int i = 0; i < dir.size(); i++) {
		if (dir[i]["name"] != util::fs::path::prev) {
			std::map<std::string, std::string> setting = util::cfg::lex("player/cfg" + util::fs::path::sep + dir[i]["name"]);

			for (std::map<std::string, std::string>::iterator it = setting.begin(); it != setting.end(); ++it) {
				if (std::find(omni::var.begin(), omni::var.end(), it->first) != omni::var.end()) {
					// init
					if (it->first == "skip_boot") {
						boot = !util::cfg::parse::boolean(it->second);
					}

					// display
					if (it->first == "screen_width") {
						int val = util::cfg::parse::whole(it->second);

						omni::assert(val >= 0, std::string("Value `") + std::to_string(val) + std::string("` invalid for option `") + it->first + std::string("`; number must be positive"));

						layout::view[X] = util::cfg::parse::whole(it->second);
					}

					if (it->first == "fps") {
						int val = util::cfg::parse::whole(it->second);

						omni::assert(val >= 0, std::string("Value `") + std::to_string(val) + std::string("` invalid for option `") + it->first + std::string("`; number must be positive"));

						state::fps = val;
					}

					if (it->first == "speed") {
						int val = util::cfg::parse::prec(it->second);

						omni::assert(val >= 0, std::string("Value `") + std::to_string(val) + std::string("` invalid for option `") + it->first + std::string("`; number must be positive"));

						state::speed = val;
					}

					if (it->first == "anim") {
						state::anim = util::cfg::parse::boolean(it->second);
					}

					if (it->first == "begin_index_at_one") {
						state::baseNo = util::cfg::parse::boolean(it->second);
					}

					// console
					if (it->first == "line_count") {
						int val = util::cfg::parse::whole(it->second);

						omni::assert(val >= 0, std::string("Value `") + std::to_string(val) + std::string("` invalid for option `") + it->first + std::string("`; number must be positive"));

						state::lineCnt = val;
						layout::view[Y] = state::lineCnt * layout::glyph[Y];
					}

					if (it->first == "line_width") {
						int val = util::cfg::parse::whole(it->second);

						omni::assert(val >= 0, std::string("Value `") + std::to_string(val) + std::string("` invalid for option `") + it->first + std::string("`; number must be positive"));

						state::lineWd = val;
					}

					if (it->first == "tab_width") {
						int val = util::cfg::parse::whole(it->second);

						omni::assert(val >= 0, std::string("Value `") + std::to_string(val) + std::string("` invalid for option `") + it->first + std::string("`; number must be positive"));

						state::tabWd = val;
					}

					if (it->first == "tab_expand") {
						state::expandTab = util::cfg::parse::boolean(it->second);
					}

					if (it->first == "start_dir") {
						state::startDir = util::cfg::parse::str(it->second);
					}

					if (it->first == "hl_line_no") {
						state::hlLineNo = util::cfg::parse::boolean(it->second);
					}

					if (it->first == "hl_active_line") {
						state::hlActiveLine = util::cfg::parse::boolean(it->second);
					}

					if (it->first == "show_file_perm") {
						state::showFilePerm = util::cfg::parse::boolean(it->second);
					}

					if (it->first == "show_diff") {
						state::showDiff = util::cfg::parse::boolean(it->second);
					}

					if (it->first == "time_format") {
						state::format = util::cfg::parse::str(it->second);
					}
				} else {
					omni::err(omni::ERR_CFG_NO_KEY, {
						it->first
					});
				}
			}
		}
	}

	layout::canv[X] = state::lineWd * layout::glyph[X];
	layout::canv[Y] = state::lineCnt * layout::glyph[Y];

	layout::res[X] = layout::canv[X] + layout::view[X];
	layout::res[Y] = layout::view[Y];

	disp = new Disp("Omni", {
		layout::res[X],
		layout::res[Y]
	}, col[false]);

	nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("stat.json"));

	if (argc > 1) {
		omni::assert(omni::stage.find(argv[1]) != omni::stage.end(), "Couldn't initialize; no corresponding section `" + std::string(argv[1]) + "`");

		stage = argv[1];
	} else {
		stage = "init";
	}
	if (argc > 2) {
		std::string name = argv[2];

		std::string path = std::string("lvl") + util::fs::path::sep + argv[1] + util::fs::path::sep + name + std::string(".json");
		std::ifstream in(path);
		omni::assert(in.good(), "Couldn't initialize; no level `" + std::to_string(std::stoi(argv[2])) + std::string("` in stage `") + argv[1] + std::string("`"));

		lvl = atoi(argv[2]);
	} else {
		lvl = 0;
	}

	std::string name;
	if (!(argc > 1)) {
		name = util::fs::path::build({
			"doc",
			"intro.txt"
		});
	}

	console = new Console(name, state::startDir);

	unsigned int rank = deser["rank"];

	if (rank >= omni::stage[stage]) {
		scn::init(stage, lvl);
	} else {
		omni::err(omni::ERR_RANK_LT, {
			stage
		});
	}

	Py_Initialize();

	PyObject* path = PySys_GetObject("path");
	PyList_Append(path, PyUnicode_FromString("."));
	PyList_Append(path, PyUnicode_FromString("player/script"));

	if (boot) {
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

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	std::thread event(handle);
	event.detach();

	SDL_AddTimer(0, epoch, NULL);

	while (disp->open) {
		if (adv) {
			lvl++;

			scn::init(stage, lvl);

			adv = false;
		}

		disp->clear();

		glViewport(0, 0, layout::canv[X], layout::canv[Y]);

		glDisable(GL_DEPTH_TEST);

		console->clear();

		console->fmtScr();
		console->hl();

		console->render();
		console->draw();

		glViewport(layout::canv[X], 0, layout::view[X], layout::view[Y]);

		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < scn::obj.size(); i++) {
			objDraw(scn::obj[i]);
		}

		disp->update();
	}

	return 0;
}
