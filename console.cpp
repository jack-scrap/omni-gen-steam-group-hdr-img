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
#include "layout.h"
#include "math.h"
#include "state.h"

Console::Console(std::string fName, std::string cwd) :
	_mode(EDITOR),
	_cwd(cwd),
	_prog("console", "tex") {
		_canv = (char*) calloc(state::ln * state::line, sizeof (char));
		_hl = (char*) calloc(state::ln * state::line, sizeof (bool));

		_data = (char*) calloc(layout::canv[X] * layout::canv[Y] * 3, sizeof (char));
		_blank = (char*) calloc(layout::canv[X] * layout::canv[Y] * 3, sizeof (char));

		for (int y = 0; y < layout::canv[Y]; y++) {
			for (int x = 0; x < layout::canv[X]; x++) {
				unsigned idx = idxStatic({
					x,
					y
				}, {
					layout::canv[X],
					layout::canv[Y]
				}) * 3;

				_blank[idx] = col[false].b;
				_blank[idx + 1] = col[false].g;
				_blank[idx + 2] = col[false].r;
			}
		}

		// cursor
		for (int y = 0; y < 100; y++) {
			for (int x = 0; x < 100; x++) {
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::ln * layout::glyph[X], state::line * layout::glyph[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, _blank);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		_prog.unUse();

		getTime();

		fmt();
		hl();

		render();
	}

void Console::fmtBuff(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr) {
	int l = ptr._y;
	int y = 0;
	while (
		l < buff.size() &&
		y < view._y &&
		y < state::line
	) {
		int c = ptr._x;
		int x = 0;
		while (
			c < buff[l].size() &&
			x < view._x &&
			x < state::ln
		) {
			_canv[idxStatic({
				loc._x + x,
				loc._y + y
			}, {
				state::ln,
				state::line
			})] = buff[l][c];

			c++;
			x++;
		}

		l++;
		y++;
	}
}

void Console::clear() {
	glBindTexture(GL_TEXTURE_2D, _tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::ln * layout::glyph[X], state::line * layout::glyph[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, _blank);

	for (int y = 0; y < state::line; y++) {
		for (int x = 0; x < state::ln; x++) {
			unsigned int idx = idxStatic({
				x,
				y
			}, {
				state::ln,
				state::line
			});

			_canv[idx] = 0;
			_hl[idx] = false;
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
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

	int i = 0;
	int x = 0;
	while (
		i < modeStr.size() &&
		x < state::ln
	) {
		_canv[idxStatic({
			x,
			0
		}, {
			state::ln,
			state::line
		})] = modeStr[i];

		i++;
		x++;
	}

	_canv[idxStatic({
		x,
		0
	}, {
		state::ln,
		state::line
	})] = ' ';
	x++;

	std::string base = util::fs::name(_buffName);
	i = 0;
	while (
		i < base.size() &&
		x < state::ln
	) {
		_canv[idxStatic({
			x,
			0
		}, {
			state::ln,
			state::line
		})] = base[i];

		i++;
		x++;
	}

	std::string time = std::string(_timeFmt);

	while (x < state::ln - time.size()) {
		_canv[idxStatic({
			x,
			0
		}, {
			state::ln,
			state::line
		})] = ' ';

		x++;
	}

	i = 0;
	while (
		i < time.size() &&
		x < state::ln
	) {
		_canv[idxStatic({
			x,
			0
		}, {
			state::ln,
			state::line
		})] = time[i];

		i++;
		x++;
	}

	loc[Y] += 1;

	const unsigned int boundFrame[2] = {
		state::ln,
		state::line - 1 - 1
	};
	const unsigned int boundPrompt = state::ln - _ps1.size();
	const unsigned int btm = state::line - 1;

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

	loc[X] += maxFs;

	/* editor */
	// line numbers
	unsigned int maxNo = std::to_string(_buff.size()).size();
	maxNo += 1; // pad

	std::vector<std::string> pre;
	for (int i = _cursEditor[_rngEditor][Y] < boundFrame[Y] ? 0 : _cursEditor[_rngEditor][Y] - (boundFrame[Y] - 1); i < _buff.size(); i++) {
		std::string line;

		unsigned int base = 0;
		if (state::initIdx) {
			base = 1;
		}

		std::string no = std::to_string(base + i);

		line += no;

		no += ' ';

		pre.push_back(no);
	}

	fmtBuff(pre, {
		loc[X],
		loc[Y]
	}, {
		maxNo,
		boundFrame[Y]
	}, {
		0,
		0
	});

	loc[X] += maxNo;

	// buffer
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

	for (int l = 0; l < state::line; l++) {
		for (int c = 0; c < state::ln; c++) {
			Coord st = {
				c,
				l
			};
			unsigned int idx = idxStatic(st, {
				state::ln,
				state::line
			});
			if (_canv[idx]) {
				print(_canv[idx], _hl[idx], st);
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

	std::sort(_tree.begin(), _tree.end(), lexicoEntry);

	_cursFs = 0;
}

void Console::switchMode(unsigned int mode) {
	_mode = mode;

	fmt();
}

void Console::ins(char c) {
	switch (_mode) {
		case EDITOR:
			if (_rngEditor) {
				del();

				_buff[_cursEditor[MIN][Y]].insert(_buff[_cursEditor[MIN][Y]].begin() + _cursEditor[MIN][X], c);
			} else {
				if (_cursEditor[_rngEditor][X] == _buff[_cursEditor[_rngEditor][Y]].size()) {
					_buff[_cursEditor[_rngEditor][Y]].push_back(c);

					_cursEditor[_rngEditor][X] = _buff[_cursEditor[_rngEditor][Y]].size();
				} else {
					_buff[_cursEditor[_rngEditor][Y]][_cursEditor[_rngEditor][X]] = c;
				}
			}

			break;

		case PROMPT:
			if (_rngPrompt) {
				del();

				_prompt.insert(_prompt.begin() + _cursPrompt[MIN], c);
			} else {
				if (_cursPrompt[_rngPrompt] == _prompt.size()) {
					_prompt.push_back(c);

					_cursPrompt[_rngPrompt] = _prompt.size();
				} else {
					_prompt[_cursPrompt[_rngPrompt]] = c;
				}
			}

			break;
	}
}

void Console::newLine() {
	std::string tail = std::string(_buff[_cursEditor[MIN][Y]].begin() + _cursEditor[MIN][X], _buff[_cursEditor[MIN][Y]].end());

	_buff[_cursEditor[MIN][Y]].erase(_buff[_cursEditor[MIN][Y]].begin() + _cursEditor[MIN][X], _buff[_cursEditor[MIN][Y]].end());

	if (
		_cursEditor[MIN][Y] == _buff.size() - 1 &&
		_cursEditor[MIN][X] == _buff[_cursEditor[MIN][Y]].size()
	) {
		_buff.push_back(tail);
	} else {
		_buff.insert(_buff.begin() + _cursEditor[MIN][Y] + 1, tail);
	}

	for (int i = 0; i < 2; i++) {
		_cursEditor[i][Y]++;
		_cursEditor[i][X] = 0;
	}
}

void Console::del() {
	switch (_mode) {
		case EDITOR: {
			if (_rngEditor) {
				unsigned int idx[2];
				for (int i = 0; i < 2; i++) {
					idx[i] = _cursEditor[MIN][i];
				}
				unsigned l = idx[Y];

				while (idx[Y] < _cursEditor[MAX][Y]) {
					if (idx[X]) {
						_buff[l].erase(_buff[l].begin() + _cursEditor[MIN][X], _buff[l].end());

						l++;
					} else {
						_buff.erase(_buff.begin() + l);
					}

					idx[Y]++;
					idx[X] = 0;
				}

				_buff[l].erase(_buff[l].begin() + idx[X], _buff[l].begin() + _cursEditor[MAX][X] + 1);

				for (int i = 0; i < 2; i++) {
					_cursEditor[MAX][i] = _cursEditor[MIN][i];
				}
			} else {
				if (_buff[_cursEditor[_rngEditor][Y]].size()) {
					if (_cursEditor[_rngEditor][X] == _buff[_cursEditor[_rngEditor][Y]].size()) {
						_buff[_cursEditor[_rngEditor][Y]].pop_back();
					} else {
						_buff[_cursEditor[_rngEditor][Y]].erase(_buff[_cursEditor[_rngEditor][Y]].begin() + _cursEditor[MIN][X], _buff[_cursEditor[_rngEditor][Y]].begin() + _cursEditor[MAX][X] + 1);
					}
				} else {
					if (_buff.size() > 1) {
						if (_cursEditor[_rngEditor][Y] == _buff.size() - 1) {
							_buff.pop_back();

							_cursEditor[_rngEditor][Y]--;

							console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
						}
					}
				}

				for (int r = 0; r < 2; r++) {
					if (!(_cursEditor[r][Y] < _buff.size())) {
						_cursEditor[r][Y]--;
					}

					if (_cursEditor[r][X] > _buff[_cursEditor[r][Y]].size()) {
						_cursEditor[r][X] = _buff[_cursEditor[r][Y]].size();
					}
				}
			}

			break;
		}

		case PROMPT:
			if (_rngPrompt) {
				unsigned int floor;
				unsigned int roof;
				if (_cursPrompt[MIN] < _cursPrompt[MAX]) {
					floor = _cursPrompt[MIN];
					roof = _cursPrompt[MAX];
				}
				if (_cursPrompt[MIN] > _cursPrompt[MAX]) {
					floor = _cursPrompt[MAX];
					roof = _cursPrompt[MIN];
				}

				roof += 1;

				if (!(roof < _prompt.size())) {
					roof = _prompt.size();
				}

				_prompt.erase(_prompt.begin() + floor, _prompt.begin() + roof);

				_cursPrompt[MAX] = _cursPrompt[MIN];
			} else {
				if (_cursPrompt[_rngPrompt] == _prompt.size()) {
					if (_prompt.size()) {
						_prompt.pop_back();
					}
				} else {
					_prompt.erase(_prompt.begin() + _cursPrompt[_rngPrompt]);
				}
			}

			if (!(_cursPrompt[MIN] < _prompt.size())) {
				_cursPrompt[MIN] = _prompt.size();
			}
			_cursPrompt[MAX] = _cursPrompt[MIN];

			break;
	}
}

void Console::exec() {
	if (_prompt.size()) {
		std::vector<std::string> tok = util::str::split(_prompt, ' ');

		std::string cmd = tok[0];

		if (std::find(omni::lib.begin(), omni::lib.end(), cmd) != omni::lib.end()) {
			if (cmd == "open") {
				if (tok.size() == 1 + 1) {
					open(tok[1]);
				} else {
					omni::err("Incorrect number of arguments to command `" + cmd + "`");
				}
			}

			if (cmd == "write") {
				if (tok.size() <= 1 + 1) {
					std::string name;
					if (tok.size() == 1) {
						name = _buffName;
					} else {
						name = tok[1];
					}

					util::fs::write(name, _buff);
				} else {
					omni::err("Incorrect number of arguments to command `" + cmd + "`");
				}
			}

			if (cmd == "del") {
				if (tok.size() <= 1 + 1) {
					std::string name;
					if (tok.size() == 1) {
						name = _buffName;
					} else {
						name = tok[1];
					}

					util::fs::del(name);
				} else {
					omni::err("Incorrect number of arguments to command `" + cmd + "`");
				}
			}
		} else {
			omni::err("Command `" + cmd + "` not found");
		}
	}

	_prompt.clear();

	_cursPrompt[MIN] = _prompt.size();
	_cursPrompt[MAX] = _cursPrompt[MIN];

	fmt();
}

void Console::hl() {
	glBindTexture(GL_TEXTURE_2D, _tex);

	for (int y = 0; y < state::line; y++) {
		for (int x = 0; x < state::ln; x++) {
			_hl[idxStatic({
				x,
				y
			}, {
				state::ln,
				state::line
			})] = false;
		}
	}

	unsigned int loc[2] = {
		0
	};

	/* status bar */
	for (int i = 0; i < state::ln; i++) {
		unsigned int idx = idxStatic({
			i,
			0
		}, {
			state::ln,
			state::line
		});
		_hl[idx] = true;
	}

	const unsigned int boundFrame[2] = {
		state::ln,
		state::line - 1 - 1
	};
	const unsigned int btm = state::line - 1;

	/* file system */
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
	loc[Y] = 1;

	/* editor */
	// line number
	unsigned int maxNo = std::to_string(_buff.size()).size() + 1;

	if (state::hlLineNo) {
		int l = 0;
		int y = 0;
		while (l < boundFrame[Y]) {
			if (l < _buff.size()) {
				int c = 0;
				int x = 0;
				while (c < maxNo) {
					unsigned int idx = idxStatic({
						loc[X] + x,
						loc[Y] + y
					}, {
						state::ln,
						state::line
					});
					_hl[idx] = true;

					c++;
					x++;
				}
			}

			l++;
			y++;
		}
	}

	if (state::hlActiveLine) {
		int
			delta = util::math::delta(_cursEditor[MIN][Y], _cursEditor[MAX][Y]),
			norm = util::math::norm(_cursEditor[MIN][Y], _cursEditor[MAX][Y]);

		for (int l = 0; l < abs(delta) + 1; l++) {
			for (int c = 0; c < maxNo; c++) {
				unsigned int idx = idxStatic({
					loc[X] + c,
					loc[Y] + _cursEditor[MIN][Y] + (l * norm)
				}, {
					state::ln,
					state::line
				});

				_hl[idx] = !_hl[idx];
			}
		}
	}

	/* cursor */
	// editor
	unsigned int boundEditor[2] = {
		boundFrame[X] - (maxFs + maxNo),
		boundFrame[Y]
	};

	switch (_mode) {
		case EDITOR: {
			loc[X] = maxFs + maxNo;
			loc[Y] = 1;

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
			for (int i = 0; i < 1 + abs(deltaConting); i++) {
				unsigned int idx = idxStatic({
					loc[X] + st[X],
					loc[Y] + st[Y]
				}, {
					state::ln,
					state::line
				});

				_hl[idx] = true;

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
					unsigned int clamped[2] = {
						clamp(loc[X] + _cursEditor[r][X], state::ln - 1),
						clamp(loc[Y] + _cursEditor[r][Y], boundFrame[Y])
					};

					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped[X] * layout::glyph[X], clamped[Y] * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case PROMPT: {
			loc[X] = _ps1.size();
			loc[Y] = btm;

			int
				delta = util::math::delta(_cursPrompt[MIN], _cursPrompt[MAX]),
				norm = util::math::norm(_cursPrompt[MIN], _cursPrompt[MAX]);

			for (int i = 0; i < 1 + abs(delta); i++) {
				unsigned int idx = idxStatic({
					loc[X] + _cursPrompt[MIN] + (i * norm),
					loc[Y]
				}, {
					state::ln,
					state::line
				});

				_hl[idx] = true;
			}

			// block
			for (int r = 0; r < 2; r++) {
				unsigned int clamped = clamp(_ps1.size() + _cursPrompt[r], state::ln - 1);

				if (_cursPrompt[r] == _prompt.size()) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped * layout::glyph[X], (state::line - 1) * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case FS: {
			loc[X] = 0;
			loc[Y] = 1;

			for (int c = 0; c < maxFs; c++) {
				unsigned int idx = idxStatic({
					loc[X] + c,
					loc[Y] + clamp(_cursFs, boundFrame[Y] - 1)
				}, {
					state::ln,
					state::line
				});

				_hl[idx] = !_hl[idx];
			}

			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Console::print(char c, bool b, Coord st) {
	unsigned int idx[2] = {
		c % 16,
		c / 16
	};

	unsigned int sz = layout::map[X] * layout::map[Y] * layout::glyph[X] * layout::glyph[Y];
	unsigned int stride[2] = {
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

	glBindTexture(GL_TEXTURE_2D, 0);
	_prog.unUse();
	glBindVertexArray(0);
}

unsigned int Console::idxStatic(Coord st, Coord bound) {
	return (st._y * bound._x) + st._x;
}

unsigned int Console::idxDeterm(std::vector<std::string> buff, Coord st) {
	unsigned int _ = 0;
	for (int i = 0; i < st._y; i++) {
		_ += _buff[i].size();
	}
	_ += st._x;

	return _;
}

unsigned int Console::clamp(unsigned int i, unsigned int roof) {
	unsigned int _ = i;
	if (!(_ < roof)) {
		_ = roof;
	}

	return _;
}

void Console::getTime() {
	time(&_raw);
	_info = localtime(&_raw);
	strftime(_timeFmt, 64, state::format.c_str(), _info);
}

bool Console::lexicoEntry(std::map<std::string, std::string> lhs, std::map<std::string, std::string> rhs) {
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
