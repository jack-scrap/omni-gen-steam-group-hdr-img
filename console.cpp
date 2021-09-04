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
	_cwd(cwd),
	_prog("console", "tex") {
		for (int i = 0; i < 2; i++) {
			_res[i] = res[i];
		}

		_canv = (char*) calloc(_res[X] * _res[Y], sizeof (char));
		_hl = (char*) calloc(_res[X] * _res[Y], sizeof (bool));

		_data = (char*) calloc(_res[X] * layout::glyph[X] * _res[Y] * layout::glyph[Y] * 3, sizeof (char));

		// cursor
		for (int y = 0; y < 32 * 3; y++) {
			for (int x = 0; x < 16 * 3; x++) {
				_block[y][x][0] = col[true].b;
				_block[y][x][1] = col[true].g;
				_block[y][x][2] = col[true].r;
			}
		}

		open(fName);

		changeDir(_cwd);

		_cursEditor[MIN][X] = _buff.back().size() - 1 + 1;
		_cursEditor[MIN][Y] = _buff.size() - 1;
		for (int i = 0; i < 2; i++) {
			_cursEditor[MAX][i] = _cursEditor[MIN][i];
		}

		_cursPrompt[MIN] = _prompt.size();
		_cursPrompt[MAX] = _cursPrompt[MIN];

		_cursFs = 0;

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

		_prog.unUse();

		fmt();
		hl();

		render();
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

void Console::clear() {
	for (int y = 0; y < _res[Y]; y++) {
		for (int x = 0; x < _res[X]; x++) {
			unsigned int idx = idxStatic({
				x,
				y
			}, _res);

			_canv[idx] = 0;
			_hl[idx] = false;
		}
	}
}

void Console::fmt() {
	clear();

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

	time(&_raw);
	_info = localtime(&_raw);
	strftime(_timeFmt, 64, state::format.c_str(), _info);

	std::string time = std::string(_timeFmt);

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

	const unsigned int
		boundFrame[2] = {
			_res[X],
			_res[Y] - 1 - 1
		},
		boundPrompt = _res[X] - _ps1.size(),
		btm = _res[Y] - 1;

	/* file system */
	unsigned int maxFs = 0;
	for (int i = 0; i < _tree.size(); i++) {
		std::string line = _tree[i]["name"];

		if (_tree[i]["type"] == "dir") {
			line += "/";
		}

		if (line.size() > maxFs) {
			maxFs = line.size();
		}
	}
	maxFs += 1; // pad

	std::vector<std::string> entryName;
	for (int i = 0; i < _tree.size(); i++) {
		std::string line = _tree[i]["name"];

		if (_tree[i]["type"] == "dir") {
			line += "/";
		}

		int c = line.size();
		while (c < maxFs) {
			line += ' ';

			c++;
		}

		entryName.push_back(line);
	}

	fmtBuff(entryName, {
		loc[X],
		loc[Y]
	}, {
		maxFs,
		boundFrame[Y]
	}, {
		0,
		_cursFs < boundFrame[Y] ? 0 : _cursFs - (boundFrame[Y] - 1)
	});

	loc[X] = maxFs;

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

	/* command-line */
	loc[X] = 0;

	fmtBuff({
		_ps1,
	}, {
		loc[X],
		btm
	}, {
		_ps1.size(),
		1
	}, {
		0,
		0
	});

	loc[X] += _ps1.size();

	fmtBuff({
		_prompt,
	}, {
		loc[X],
		btm
	}, {
		boundPrompt,
		1
	}, {
		_cursPrompt[_rngPrompt] < boundPrompt ? 0 : _cursPrompt[_rngPrompt] - (boundPrompt - 1),
		0
	});
}

void Console::render() {
	glBindTexture(GL_TEXTURE_2D, _tex);

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

	glBindTexture(GL_TEXTURE_2D, 0);
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

void Console::hl() {
	for (int y = 0; y < _res[Y]; y++) {
		for (int x = 0; x < _res[X]; x++) {
			_hl[idxStatic({
				x,
				y
			}, _res)] = false;
		}
	}

	unsigned int loc[2] = {
		0
	};

	/* status bar */
	for (int i = 0; i < _res[X]; i++) {
		unsigned int idx = idxStatic({
			i,
			0
		}, _res);
		_hl[idx] = true;
	}

	const unsigned int boundFrame[2] = {
		_res[X],
		_res[Y] - 1 - 1
	};

	unsigned int maxFs = 0;
	for (int i = 0; i < _tree.size(); i++) {
		std::string fmt = _tree[i]["name"];
		if (_tree[i]["type"] == "dir") {
			fmt += "/";
		}

		if (fmt.size() > maxFs) {
			maxFs = fmt.size();
		}
	}
	maxFs += 1;

	loc[X] += maxFs;

	unsigned int maxNo = std::to_string(_buff.size()).size() + 1;
	int
		l = 0,
		y = 0;
	while (l < boundFrame[Y]) {
		if (l < _buff.size()) {
			int
				c = 0,
				x = 0;
			while (c < maxNo) {
				unsigned int idx = idxStatic({
					loc[X] + x,
					loc[Y] + y
				}, _res);
				_hl[idx] = !_hl[idx];

				c++;
				x++;
			}
		}

		l++;
		y++;
	}

	if (state::hlLineNo) {
		int
			delta = util::math::delta(_cursEditor[MIN][Y], _cursEditor[MAX][Y]),
			norm = util::math::norm(_cursEditor[MIN][Y], _cursEditor[MAX][Y]);

		for (int l = 0; l < abs(delta) + 1; l++) {
			for (int c = 0; c < maxNo; c++) {
				unsigned int idx = idxStatic({
					loc[X] + c,
					loc[Y] + _cursEditor[MIN][Y] + (l * norm)
				}, _res);

				_hl[idx] = !_hl[idx];
			}
		}
	}

	/* cursor */
	loc[X] += maxNo;

	// editor
	unsigned int boundEditor[2] = {
		boundFrame[X] - (maxFs + maxNo),
		boundFrame[Y]
	};

	switch (_mode) {
		case EDITOR: {
			int
				delta = util::math::delta(_cursEditor[MIN][X] + _cursEditor[MIN][Y], _cursEditor[MAX][X] + _cursEditor[MAX][Y]),
				norm = util::math::norm(idxDeterm(_buff, {
					_cursEditor[MIN][X],
					_cursEditor[MIN][Y]
				}), idxDeterm(_buff, {
					_cursEditor[MAX][X],
					_cursEditor[MAX][Y]
				})),

				deltaConting = util::math::delta(idxDeterm(_buff, {
					_cursEditor[MIN][X],
					_cursEditor[MIN][Y]
				}), idxDeterm(_buff, {
					_cursEditor[MAX][X],
					_cursEditor[MAX][Y]
				}));

			unsigned int st[2];
			for (int i = 0; i < 2; i++) {
				st[i] = _cursEditor[MIN][i];
			}
			for (int i = 0; i < abs(deltaConting) + 1; i++) {
				unsigned int clamped[2];
				for (int i = 0; i < 2; i++) {
					if (st[i] < boundEditor[i] - 1) {
						clamped[i] = st[i];
					} else {
						clamped[i] = boundEditor[i] - 1;
					}
				}

				_hl[idxStatic({
					loc[X] + clamped[X],
					loc[Y] + clamped[Y]
				}, _res)] = true;

				if (norm == 1) {
					if (st[X] < _buff[st[Y]].size() - 1) {
						st[X]++;
					} else {
						if (st[Y] != _cursEditor[MAX][Y]) {
							st[Y]++;
							st[X] = 0;
						}
					}
				}
				if (norm == -1) {
					if (st[X] > 0) {
						st[X]--;
					} else {
						if (st[Y] != _cursEditor[MAX][Y]) {
							st[Y]--;
							st[X] = _buff[st[Y]].size() - 1;
						}
					}
				}
			}

			// block
			for (int r = 0; r < 2; r++) {
				if (_cursEditor[r][X] == _buff[_cursEditor[r][Y]].size()) {
					unsigned int clamped[2];
					if (loc[X] + _cursEditor[r][X] < _res[X]) {
						clamped[X] = loc[X] + _cursEditor[r][X];
					} else {
						clamped[X] = _res[X] - 1;
					}
					if (loc[Y] + _cursEditor[r][Y] < boundFrame[Y] + 1) {
						clamped[Y] = loc[Y] + _cursEditor[r][Y];
					} else {
						clamped[Y] = boundFrame[Y];
					}

					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped[X] * layout::glyph[X], clamped[Y] * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case PROMPT: {
			int
				delta = util::math::delta(_cursPrompt[MIN], _cursPrompt[MAX]),
				norm = util::math::norm(_cursPrompt[MIN], _cursPrompt[MAX]);

			for (int i = 0; i < abs(delta) + 1; i++) {
				unsigned int idx = idxStatic({
					_ps1.size() + _cursPrompt[MIN] + (i * norm),
					_res[Y] - 1
				}, _res);

				_hl[idx] = !_hl[idx];
			}

			// block
			for (int r = 0; r < 2; r++) {
				unsigned int cursLocal;
				if (_ps1.size() + _cursPrompt[r] < _res[X]) {
					cursLocal = _ps1.size() + _cursPrompt[r];
				} else {
					cursLocal = _res[X] - 1;
				}

				if (_cursPrompt[r] == _prompt.size()) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, cursLocal * layout::glyph[X], (_res[Y] - 1) * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case FS: {
			unsigned int cursLocal;
			if (_cursFs < boundFrame[Y] - 1) {
				cursLocal = _cursFs;
			} else {
				cursLocal = boundFrame[Y] - 1;
			}

			for (int i = 0; i < maxFs; i++) {
				unsigned int idx = idxStatic({
						i,
						cursLocal
						}, _res);
				_hl[idx] = !_hl[idx];
			}

			break;
		}
	}
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

unsigned int Console::idxDeterm(std::vector<std::string> buff, Coord st) {
	unsigned int _ = 0;
	for (int i = 0; i < st._y; i++) {
		_ += _buff[i].size();
	}
	_ += st._x;

	return _;
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
