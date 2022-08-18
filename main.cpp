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
#include "class_def.h"

Disp* disp;
Console* console;

Cam cam;

bool boot = false;

extern std::string stage;
extern unsigned int lvl;

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

	return 0;
}
