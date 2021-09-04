#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "console.h"
#include "omni.h"
#include "mesh.h"
#include "scn.h"
#include "disp.h"
#include "util.h"
#include "col.h"
#include "stb_image.h"
#include "layout.h"
#include "math.h"

Console::Console(std::string fName, std::string cwd, unsigned int res[2]) :
	_prog("console", "tex") {
		for (int i = 0; i < 2; i++) {
			_res[i] = res[i];
		}

		_canv = (char*) calloc(_res[X] * _res[Y], sizeof (char));
		_data = (char*) calloc(_res[X] * layout::glyph[X] * _res[Y] * layout::glyph[Y] * 3, sizeof (char));

		open(fName);

		changeDir(_cwd);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 2 * sizeof (GLfloat), _vtc, GL_STATIC_DRAW);

		glGenBuffers(1, &_stbo);
		glBindBuffer(GL_ARRAY_BUFFER, _stbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 2 * sizeof (GLfloat), _st, GL_STATIC_DRAW);

		glGenBuffers(1, &_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof (GLushort), _idc, GL_STATIC_DRAW);

		_prog.use();

		// attribute
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		_attrPos = glGetAttribLocation(_prog._id, "pos");
		glVertexAttribPointer(_attrPos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attrPos);

		glBindBuffer(GL_ARRAY_BUFFER, _stbo);
		_attrSt = glGetAttribLocation(_prog._id, "st");
		glVertexAttribPointer(_attrSt, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attrSt);

		// texture
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _res[X] * layout::glyph[X], _res[Y] * layout::glyph[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, _data);

		glGenerateMipmap(GL_TEXTURE_2D);

		fmt();
		render();

		_prog.unUse();
	}

void Console::fmtBuff(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr) {
	int
		l = ptr._y,
		y = 0;
	while (
		l < buff.size() &&
		y < view._y &&
		y < _res[Y]
	) {
		int
			c = ptr._x,
			x = 0;
		while (
			c < buff[l].size() &&
			x < view._x &&
			x < _res[X]
		) {
			_canv[idxStatic({
				loc._x + x,
				loc._y + y
			}, _res)] = buff[l][c];

			c++;
			x++;
		}

		l++;
		y++;
	}
}

void Console::fmt() {
	unsigned int loc[2] = {
		0
	};

	/* status bar */
	std::string modeStr;
	switch (_mode) {
		case EDITOR:
			modeStr = "EDITOR";

			break;

		case PROMPT:
			modeStr = "PROMPT";

			break;

		case FS:
			modeStr = "FS";

			break;
	}

	int
		i = 0,
		x = 0;
	while (
		i < modeStr.size() &&
		x < _res[X]
	) {
		_canv[idxStatic({
			x,
			0
		}, _res)] = modeStr[i];

		i++;
		x++;
	}

	_canv[idxStatic({
		x,
		0
	}, _res)] = ' ';
	x++;

	std::string base = util::fs::name(_buffName);
	i = 0;
	while (
		i < base.size() &&
		x < _res[X]
	) {
		_canv[idxStatic({
			x,
			0
		}, _res)] = base[i];

		i++;
		x++;
	}

	std::string time = "asdf";

	while (x < _res[X] - time.size()) {
		_canv[idxStatic({
			x,
			0
		}, _res)] = ' ';

		x++;
	}

	i = 0;
	while (
		i < time.size() &&
		x < _res[X]
	) {
		_canv[idxStatic({
			x,
			0
		}, _res)] = time[i];

		i++;
		x++;
	}

	loc[Y] += 1;

	const unsigned int boundFrame[2] = {
		_res[X],
		_res[Y] - 1 - 1
	};

	/* editor */
	fmtBuff(_buff, {
		loc[X],
		loc[Y]
	}, {
		boundFrame[X],
		boundFrame[Y]
	}, {
		0,
		0
	});
}

void Console::render() {
	for (int l = 0; l < _res[Y]; l++) {
		for (int c = 0; c < _res[X]; c++) {
			Coord st = {
				c,
				l
			};
			unsigned int idx = idxStatic(st, _res);
			if (_canv[idx]) {
				print(_canv[idx], false, st);
			}
		}
	}
}

void Console::open(std::string fName) {
	_buffName = fName;

	_buff = util::fs::rd<std::vector<std::string>>(_buffName);
}

void Console::changeDir(std::string dir) {
	_cwd = dir;

	_tree = util::fs::ls(_cwd);

	for (int i = 0; i < _tree.size(); i++) {
		if (_tree[i]["name"] == "..") {
			if (_cwd == "player") {
				_tree.erase(_tree.begin() + i);
			}
		}
	}

	std::sort(_tree.begin(), _tree.end(), numeric);

	_cursFs = 0;
}

void Console::print(char c, bool b, Coord st) {
	unsigned int
		idx[2] = {
			c % 16,
			c / 16
		},

		sz = layout::map[X] * layout::map[Y] * layout::glyph[X] * layout::glyph[Y],
		stride[2] = {
			layout::glyph[X],
			layout::map[X] * layout::glyph[X] * layout::glyph[Y]
		};

	glTexSubImage2D(GL_TEXTURE_2D, 0, st._x * layout::glyph[X], st._y * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, &(((char*) disp->_map->pixels)[((b * sz) + (idx[Y] * stride[Y]) + (idx[X] * stride[X])) * 3]));
}

void Console::draw() {
	glBindVertexArray(_vao);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	_prog.unUse();
	glBindVertexArray(0);
}

unsigned int Console::idxStatic(Coord st, unsigned int bound[2]) {
	return (st._y * bound[X]) + st._x;
}


bool Console::numeric(std::map<std::string, std::string> lhs, std::map<std::string, std::string> rhs) {
	std::string::iterator it[2] = {
		lhs["name"].begin(),
		rhs["name"].begin()
	};

	if (
		std::isdigit(lhs["name"][0]) &&
		std::isdigit(rhs["name"][0])
	) {
		int n[2];

		std::stringstream ss(lhs["name"]);
		ss >> n[0];

		ss.clear();
		ss.str(rhs["name"]);
		ss >> n[1];

		if (n[0] != n[1]) {
			return n[0] < n[1];
		}

		it[0] = std::find_if(lhs["name"].begin(), lhs["name"].end(), [](char c) {
			return !std::isdigit(c);
		});
		it[1] = std::find_if(rhs["name"].begin(), rhs["name"].end(), [](char c) {
			return !std::isdigit(c);
		});
	}

	return std::lexicographical_compare(it[0], lhs["name"].end(), it[1], rhs["name"].end());
}
