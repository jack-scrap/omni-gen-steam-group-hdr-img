#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <Python.h>
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
#include "dict.h"

std::string stage;
unsigned int lvl;

uint32_t epoch(uint32_t inter, void* param) {
	console->getTime();
	console->fmt();

	return 1000;
}

void dispatch(std::string fName) {
	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(fName);

	PyRun_SimpleString(util::str::join(buff).c_str());

	bool eq = true;
	for (int i = 0; i < noData; i++) {
		switch (type[i]) {
			case omni::SCALAR:
				if (!idxEq((Idx*) data[i]->_ptr, (Idx*) goal[i]->_ptr)) {
					eq = false;
				}

				break;

			case omni::ARRAY:
				if (!arrayEq((Array*) data[i]->_ptr, (Array*) goal[i]->_ptr)) {
					eq = false;
				}

				break;

			case omni::DICT:
				if (!dictEq((Dict*) data[i]->_ptr, (Dict*) goal[i]->_ptr)) {
					eq = false;
				}

				break;
		}
	}
	
	if (eq) {
		std::string log = "log/" + util::fs::base(fName) + ".log";

		console->_buff = util::log(console->_buff.size());

		nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("player.json"));

		unsigned int rank = serial["rank"];
		rank++;

		nlohmann::json data = {
			{
				"rank", rank
			}
		};

		std::string deser = data.dump();

		util::fs::write("player.json", util::str::split(deser, '\n'));
	}
}

Console::Console(std::string fName, std::string cwd) :
	_mode(EDITOR),
	_cwd(cwd),
	_prog("console", "tex") {
		_canv = (char*) calloc(state::lineWd * state::lineCnt, sizeof (char));
		_hl = (char*) calloc(state::lineWd * state::lineCnt, sizeof (bool));

		_data = (char*) calloc(layout::canv[X] * layout::canv[Y] * 3, sizeof (char));
		_blank = (char*) calloc(layout::canv[X] * layout::canv[Y] * 3, sizeof (char));

		for (int y = 0; y < layout::canv[Y]; y++) {
			for (int x = 0; x < layout::canv[X]; x++) {
				unsigned idx = util::math::idx::arr({
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

		glGenVertexArrays(1, &_id[Mesh::VAO]);
		glBindVertexArray(_id[Mesh::VAO]);

		glGenBuffers(1, &_id[Mesh::VBO]);
		glBindBuffer(GL_ARRAY_BUFFER, _id[Mesh::VBO]);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 2 * sizeof (GLfloat), _vtc, GL_STATIC_DRAW);

		glGenBuffers(1, &_id[Mesh::STBO]);
		glBindBuffer(GL_ARRAY_BUFFER, _id[Mesh::STBO]);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 2 * sizeof (GLfloat), _st, GL_STATIC_DRAW);

		glGenBuffers(1, &_id[Mesh::IBO]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id[Mesh::IBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof (GLushort), _idc, GL_STATIC_DRAW);

		_prog.use();

		// attribute
		glBindBuffer(GL_ARRAY_BUFFER, _id[Mesh::VBO]);
		_attr[POS] = glGetAttribLocation(_prog._id, "pos");
		glVertexAttribPointer(_attr[POS], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[POS]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[Mesh::STBO]);
		_attr[ST] = glGetAttribLocation(_prog._id, "st");
		glVertexAttribPointer(_attr[ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[ST]);

		// texture
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::lineWd * layout::glyph[X], state::lineCnt * layout::glyph[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, _blank);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		_prog.unUse();

		// draw
		getTime();

		fmt();
		hl();

		render();

		// timer
		SDL_AddTimer(0, epoch, NULL);
	}

void Console::fmtBuff(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr) {
	int l = ptr._y;
	int y = 0;
	while (
		l < buff.size() &&
		y < view._y &&
		y < state::lineCnt
	) {
		int c = ptr._x;
		int x = 0;
		while (
			c < buff[l].size() &&
			x < view._x &&
			x < state::lineWd
		) {
			if (buff[l][c] == '\t') {
				int t = 0;
				while (
					t < state::tabWd &&
					x < view._x &&
					x < state::lineWd
				) {
					_canv[util::math::idx::arr({
						loc._x + x,
						loc._y + y
					}, {
						state::lineWd,
						state::lineCnt
					})] = ' ';

					t++;
					x++;
				}
			} else {
				_canv[util::math::idx::arr({
					loc._x + x,
					loc._y + y
				}, {
					state::lineWd,
					state::lineCnt
				})] = buff[l][c];

				x++;
			}

			c++;
		}

		l++;
		y++;
	}
}

void Console::clear() {
	for (int y = 0; y < state::lineCnt; y++) {
		for (int x = 0; x < state::lineWd; x++) {
			unsigned int idx = util::math::idx::arr({
				x,
				y
			}, {
				state::lineWd,
				state::lineCnt
			});

			_canv[idx] = 0;
			_hl[idx] = false;
		}
	}

	glBindTexture(GL_TEXTURE_2D, _tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::lineWd * layout::glyph[X], state::lineCnt * layout::glyph[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, _blank);

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
		x < state::lineWd
	) {
		_canv[util::math::idx::arr({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = modeStr[i];

		i++;
		x++;
	}

	_canv[util::math::idx::arr({
		x,
		0
	}, {
		state::lineWd,
		state::lineCnt
	})] = ' ';
	x++;

	std::string base = util::fs::name(_buffName);
	i = 0;
	while (
		i < base.size() &&
		x < state::lineWd
	) {
		_canv[util::math::idx::arr({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = base[i];

		i++;
		x++;
	}

	std::string time = std::string(_timeFmt);

	while (x < state::lineWd - time.size()) {
		_canv[util::math::idx::arr({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = ' ';

		x++;
	}

	i = 0;
	while (
		i < time.size() &&
		x < state::lineWd
	) {
		_canv[util::math::idx::arr({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = time[i];

		i++;
		x++;
	}

	loc[Y] += 1;

	const unsigned int boundFrame[2] = {
		state::lineWd,
		state::lineCnt - 1 - 1
	};
	const unsigned int boundPrompt = state::lineWd - _ps1.size();
	const unsigned int btm = state::lineCnt - 1;

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

		for (int i = no.size(); i < maxNo; i++) {
			no += ' ';
		}

		line += no;

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
		(loc[X] + _cursEditor[_rngEditor][X]) - (util::math::clamp(loc[X] + _cursEditor[_rngEditor][X], 1, boundFrame[X] - 1)),
		(_cursEditor[_rngEditor][Y]) - (util::math::clamp(_cursEditor[_rngEditor][Y], 1, boundFrame[Y] - 1))
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
		_cursPrompt[_rngPrompt] - util::math::clamp(_cursPrompt[_rngPrompt], 1, boundPrompt - 1),
		0
	});
}

void Console::render() {
	glBindTexture(GL_TEXTURE_2D, _tex);

	for (int l = 0; l < state::lineCnt; l++) {
		for (int c = 0; c < state::lineWd; c++) {
			Coord st = {
				c,
				l
			};
			unsigned int idx = util::math::idx::arr(st, {
				state::lineWd,
				state::lineCnt
			});
			if (_canv[idx]) {
				print(_canv[idx], _hl[idx], st);
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Console::open(std::string fName) {
	if (util::fs::exist(fName)) {
		_buffName = fName;

		std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(_buffName);

		if (buff.size()) {
			_buff = buff;
		} else {
			_buff = {
				""
			};
		}
	} else {
		omni::err("Could not open " + fName + "; file not found");
	}
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

			_diff = true;

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

	_diff = true;
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

			_diff = true;

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
					if (!_diff) {
						open(tok[1]);
					} else {
						omni::err("Buffer not saved");
					}
				} else {
					omni::err("Incorrect number of arguments to command `" + cmd + "`");
				}
			}

			if (cmd == "new") {
				if (tok.size() == 1 + 1) {
					if (!_diff) {
						_buffName = tok[1];
						_buff = {
							""
						};
					} else {
						omni::err("Buffer not saved");
					}
				} else {
					omni::err("Incorrect number of arguments to command `" + cmd + "`");
				}
			}

			if (cmd == "save") {
				if (tok.size() <= 1 + 1) {
					std::string name;
					if (tok.size() == 1) {
						name = _buffName;
					} else {
						name = tok[1];
					}

					util::fs::write(name, _buff);

					_diff = false;
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

			if (cmd == "run") {
				if (tok.size() > 1) {
					std::thread t(dispatch, tok[1]);
					t.detach();
				} else {
					std::thread t(dispatch, _buffName);
					t.detach();
				}
			}

			if (cmd == "next") {
				if (eq) {
					lvl++;

					std::string name = "player/script/" + stage + "/" + std::to_string(lvl) + "/main.py";

					console->open(name);

					scn::init(stage, lvl);

					eq = false;
				} else {
					omni::err("Couldn't proceed -- current level not complete");
				}
			}

			if (cmd == "quit") {
				if (!_diff) {
					exit(0);
				} else {
					omni::err("Buffer not saved");
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

	for (int y = 0; y < state::lineCnt; y++) {
		for (int x = 0; x < state::lineWd; x++) {
			_hl[util::math::idx::arr({
				x,
				y
			}, {
				state::lineWd,
				state::lineCnt
			})] = false;
		}
	}

	unsigned int loc[2] = {
		0
	};

	/* status bar */
	for (int i = 0; i < state::lineWd; i++) {
		unsigned int idx = util::math::idx::arr({
			i,
			0
		}, {
			state::lineWd,
			state::lineCnt
		});
		_hl[idx] = true;
	}

	const unsigned int boundFrame[2] = {
		state::lineWd,
		state::lineCnt - 1 - 1
	};
	const unsigned int btm = state::lineCnt - 1;

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
	unsigned int maxNo = std::to_string(_buff.size()).size();
	maxNo += 1; // pad

	if (state::hlLineNo) {
		int l = 0;
		int y = 0;
		while (l < boundFrame[Y]) {
			if (l < _buff.size()) {
				int c = 0;
				int x = 0;
				while (c < maxNo) {
					unsigned int idx = util::math::idx::arr({
						loc[X] + x,
						loc[Y] + y
					}, {
						state::lineWd,
						state::lineCnt
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
		int delta = util::math::delta(_cursEditor[MIN][Y], _cursEditor[MAX][Y]);
		int norm = util::math::norm(_cursEditor[MIN][Y], _cursEditor[MAX][Y]);

		for (int l = 0; l < abs(delta) + 1; l++) {
			for (int c = 0; c < maxNo; c++) {
				unsigned int idx = util::math::idx::arr({
					loc[X] + c,
					loc[Y] + _cursEditor[MIN][Y] + (l * norm)
				}, {
					state::lineWd,
					state::lineCnt
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

			int delta = util::math::idx::determ({
				_cursEditor[MAX][X],
				_cursEditor[MAX][Y]
			}, _buff) - util::math::idx::determ({
				_cursEditor[MIN][X],
				_cursEditor[MIN][Y]
			}, _buff);

			int norm = 0;
			if (delta > 0) {
				norm = 1;
			}
			if (delta < 0) {
				norm = -1;
			}

			Coord start = {
				_cursEditor[MIN][X],
				_cursEditor[MIN][Y]
			};
			unsigned int i = 0;
			unsigned int startScal = util::math::idx::determ({
				_cursEditor[MIN][X],
				_cursEditor[MIN][Y]
			}, _buff);
			while (i < 1 + abs(delta)) {
				Coord st = util::math::coord::determ(startScal + (i * norm), _buff);

				if (
					i == 1 + abs(delta) - 1 &&
					_cursEditor[MAX][X] == _buff[_cursEditor[MAX][Y]].size()
				) {
					break;
				}

				_hl[util::math::idx::arr({
					loc[X] + st._x,
					loc[Y] + st._y
				}, {
					state::lineWd,
					state::lineCnt
				})] = true;

				i++;
			}

			// block
			for (int r = 0; r < 2; r++) {
				if (_cursEditor[r][X] == _buff[_cursEditor[r][Y]].size()) {
					unsigned int clamped[2] = {
						util::math::clamp(loc[X] + _cursEditor[r][X], 1, boundFrame[X] - 1),
						util::math::clamp(loc[Y] + _cursEditor[r][Y], 1, boundFrame[Y])
					};

					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped[X] * layout::glyph[X], clamped[Y] * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case PROMPT: {
			loc[X] = _ps1.size();
			loc[Y] = btm;

			int delta = util::math::delta(_cursPrompt[MIN], _cursPrompt[MAX]);
			int norm = util::math::norm(_cursPrompt[MIN], _cursPrompt[MAX]);

			for (int i = 0; i < 1 + abs(delta); i++) {
				unsigned int idx = util::math::idx::arr({
					loc[X] + _cursPrompt[MIN] + (i * norm),
					loc[Y]
				}, {
					state::lineWd,
					state::lineCnt
				});

				_hl[idx] = true;
			}

			// block
			for (int r = 0; r < 2; r++) {
				unsigned int clamped = util::math::clamp(loc[X] + _cursPrompt[r], 1, state::lineWd - 1);

				if (_cursPrompt[r] == _prompt.size()) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped * layout::glyph[X], (state::lineCnt - 1) * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case FS: {
			loc[X] = 0;
			loc[Y] = 1;

			for (int c = 0; c < maxFs; c++) {
				unsigned int idx = util::math::idx::arr({
					loc[X] + c,
					loc[Y] + util::math::clamp(_cursFs, 1, boundFrame[Y] - 1)
				}, {
					state::lineWd,
					state::lineCnt
				});

				_hl[idx] = !_hl[idx];
			}

			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Console::print(char c, bool b, Coord st) {
	Coord idx = util::math::coord::arr(c, {
		16,
		16
	});

	unsigned int sz = layout::map[X] * layout::map[Y] * layout::glyph[X] * layout::glyph[Y];
	unsigned int stride[2] = {
		layout::glyph[X],
		layout::map[X] * layout::glyph[X] * layout::glyph[Y]
	};

	glTexSubImage2D(GL_TEXTURE_2D, 0, st._x * layout::glyph[X], st._y * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, &(((char*) disp->_map->pixels)[((b * sz) + (idx._y * stride[Y]) + (idx._x * stride[X])) * 3]));
}

void Console::draw() {
	glBindVertexArray(_id[Mesh::VAO]);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	_prog.unUse();
	glBindVertexArray(0);
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
