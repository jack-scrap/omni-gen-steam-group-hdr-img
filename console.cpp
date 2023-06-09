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
#include <SDL2/SDL_image.h>

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
	console->fmtScr();

	return 1000;
}

void dispatch(std::string fName, unsigned int ptrEditorX) {
	mtx = true;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(fName);

	PyRun_SimpleString(util::str::join(buff).c_str());

	eq = true;
	for (int i = 0; i < noData; i++) {
		switch (data[i]->type) {
			case omni::IDX:
				if (!idxEq((Idx*) data[i]->ptr, (Idx*) goal[i]->ptr)) {
					eq = false;
				}

				break;

			case omni::ARRAY:
				if (!arrayEq((Array*) data[i]->ptr, (Array*) goal[i]->ptr)) {
					eq = false;
				}

				break;

			case omni::DICT:
				if (!dictEq((Dict*) data[i]->ptr, (Dict*) goal[i]->ptr)) {
					eq = false;
				}

				break;
		}
	}

	if (eq) {
		std::string log = util::fs::path::build({
			"log",
			util::fs::path::base(fName) + ".log"
		});

		console->_buff = console->log();

		nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("stat.json"));

		if (!deser["fin"][stage]["lvl"][lvl]) {
			deser["fin"][stage]["lvl"][lvl] = true;

			unsigned int rank = deser["rank"];

			rank++;

			nlohmann::json data = {
				{
					"rank",
					rank
				}, {
					"fin",
					deser["fin"]
				}
			};

			std::string serial = data.dump(1, '\t');

			util::fs::write(util::fs::path::build({
				"stat.json"
			}), util::str::split(serial, '\n'));
		}
	}

	mtx = false;
}

Console::Console(std::string fName, std::string dir) :
	_mode(EDITOR),
	_cwd(dir),
	_prog("console", "console") {
		_map = IMG_Load("res/map.bmp");
		SDL_CreateTextureFromSurface(disp->rend, _map);

		_canv = (char*) calloc(state::lineWd * state::lineCnt, sizeof (char));
		_hl = (char*) calloc(state::lineWd * state::lineCnt, sizeof (bool));

		_data = (char*) calloc(layout::canv[X] * layout::canv[Y] * 3, sizeof (char));
		_blank = (char*) calloc(layout::canv[X] * layout::canv[Y] * 3, sizeof (char));

		// canvas
		for (int y = 0; y < layout::canv[Y]; y++) {
			for (int x = 0; x < layout::canv[X]; x++) {
				unsigned idx = util::math::idx::array({
					x,
					y
				}, {
					layout::canv[X],
					layout::canv[Y]
				}) * 3;

				_blank[idx] = col[false].r;
				_blank[idx + 1] = col[false].g;
				_blank[idx + 2] = col[false].b;
			}
		}

		// cursor
		for (int y = 0; y < sizeof _block / sizeof *_block; y++) {
			for (int x = 0; x < sizeof *_block / sizeof **_block; x++) {
				_block[y][x][0] = col[true].r;
				_block[y][x][1] = col[true].g;
				_block[y][x][2] = col[true].b;
			}
		}

		if (!fName.empty()) {
			open(fName);
		}

		changeDir(_cwd);

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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::lineWd * layout::glyph[X], state::lineCnt * layout::glyph[Y], 0, GL_BGR, GL_UNSIGNED_BYTE, _blank);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		_prog.unUse();

		// draw
		getTime();

		fmtScr();
		hl();

		render();
	}

void Console::fmt(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr) {
	int l = ptr.y;
	int y = 0;
	while (
		l < buff.size() &&
		y < view.y &&
		y < state::lineCnt
	) {
		int c = ptr.x;
		int x = 0;
		while (
			c < buff[l].size() &&
			x < view.x &&
			x < state::lineWd
		) {
			// tab expansion
			if (buff[l][c] == '\t') {
				int t = 0;
				while (
					t < state::tabWd &&
					x < view.x &&
					x < state::lineWd
				) {
					_canv[util::math::idx::array({
						loc.x + x,
						loc.y + y
					}, {
						state::lineWd,
						state::lineCnt
					})] = ' ';

					t++;
					x++;
				}
			} else {
				_canv[util::math::idx::array({
					loc.x + x,
					loc.y + y
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
			unsigned int idx = util::math::idx::array({
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::lineWd * layout::glyph[X], state::lineCnt * layout::glyph[Y], 0, GL_BGR, GL_UNSIGNED_BYTE, _blank);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Console::fmtScr() {
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
	while (i < modeStr.size() && x < state::lineWd) {
		_canv[util::math::idx::array({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = modeStr[i];

		i++;
		x++;
	}

	_canv[util::math::idx::array({
		x,
		0
	}, {
		state::lineWd,
		state::lineCnt
	})] = ' '; // pad
	x++;

	std::string fInfo;

	if (!_buffName.empty()) {
		fInfo += util::fs::path::name(util::fs::path::build({
			_home,
			_buffName
		}));
		fInfo += ' '; // pad
	}

	if (state::showFilePerm) {
		std::string permFmt = _perm;

		// wrap
		permFmt.insert(0, "[");
		permFmt += "]";

		fInfo += permFmt;
		fInfo += ' '; // pad
	}

	if (state::showDiff) {
		if (_diff) {
			fInfo += '~';
			fInfo += ' '; // pad
		}
	}

	i = 0;
	while (i < fInfo.size() && x < state::lineWd) {
		_canv[util::math::idx::array({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = fInfo[i];

		i++;
		x++;
	}

	std::string time = std::string(_timeFmt);

	unsigned roof = 0;
	if (state::lineWd > time.size()) {
		roof = state::lineWd - time.size();
	}

	// pad
	while (x < roof) {
		_canv[util::math::idx::array({
			x,
			0
		}, {
			state::lineWd,
			state::lineCnt
		})] = ' ';

		x++;
	}

	i = 0;
	while (i < time.size() && x < state::lineWd) {
		_canv[util::math::idx::array({
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
	std::vector<std::string> entryName;
	for (int i = 0; i < _tree.size(); i++) {
		std::string line = _tree[i]["name"];

		if (_tree[i]["type"] == "dir") {
			line += util::fs::path::sep;
		}

		int c = line.size();
		while (c < maxFs()) {
			line += ' ';

			c++;
		}

		entryName.push_back(line);
	}

	fmt(entryName, {
		loc[X],
		loc[Y]
	}, {
		maxFs(),
		boundFrame[Y]
	}, {
		0,
		_cursFs < boundFrame[Y] ? 0 : _cursFs - (boundFrame[Y] - 1)
	});

	loc[X] += maxFs();

	/* editor */
	// line numbers
	std::vector<std::string> pre;
	for (int i = _cursEditor[_rngEditor][Y] < boundFrame[Y] ? 0 : _cursEditor[_rngEditor][Y] - (boundFrame[Y] - 1); i < _buff.size(); i++) {
		std::string line;

		unsigned int base = 0;
		if (state::baseNo) {
			base = 1;
		}

		std::string no = std::to_string(base + i);

		for (int i = no.size(); i < maxNo(); i++) {
			no += ' '; // pad
		}

		line += no;

		pre.push_back(no);
	}

	fmt(pre, {
		loc[X],
		loc[Y]
	}, {
		maxNo(),
		boundFrame[Y]
	}, {
		0,
		0
	});

	loc[X] += maxNo();

	// buffer
	const unsigned int boundEditor[2] = {
		boundFrame[X] - (maxFs() + maxNo()),
		boundFrame[Y]
	};

	fmt(_buff, {
		loc[X],
		loc[Y]
	}, {
		boundEditor[X],
		boundEditor[Y]
	}, {
		(loc[X] + _cursEditor[_rngEditor][X]) - (util::math::clamp<unsigned int>(loc[X] + _cursEditor[_rngEditor][X], 1, boundFrame[X] - 1)),
		(_cursEditor[_rngEditor][Y]) - (util::math::clamp<unsigned int>(_cursEditor[_rngEditor][Y], 1, boundFrame[Y] - 1))
	});

	/* command-line */
	loc[X] = 0;

	fmt({
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

	fmt({
		_prompt,
	}, {
		loc[X],
		btm
	}, {
		boundPrompt,
		1
	}, {
		_cursPrompt[_rngPrompt] - util::math::clamp<unsigned int>(_cursPrompt[_rngPrompt], 1, boundPrompt - 1),
		0
	});
}

unsigned int Console::maxFs() {
	unsigned int _ = 0;
	for (int i = 0; i < _tree.size(); i++) {
		std::string line = _tree[i]["name"];

		if (_tree[i]["type"] == "dir") {
			line += util::fs::path::sep;
		}

		if (line.size() > _) {
			_ = line.size();
		}
	}

	_ += 1; // pad

	return _;
}

unsigned int Console::maxNo() {
	unsigned int _ = std::to_string(state::baseNo + _buff.size() - 1).size();

	_ += 1; // pad

	return _;
}

void Console::render() {
	glBindTexture(GL_TEXTURE_2D, _tex);

	for (int l = 0; l < state::lineCnt; l++) {
		for (int c = 0; c < state::lineWd; c++) {
			Coord st = {
				c,
				l
			};
			unsigned int idx = util::math::idx::array(st, {
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
	if (util::fs::exist(util::fs::path::build({
		_home,
		fName
	}))) {
		_buffName = fName;
		_perm = util::fs::perm(util::fs::path::build({
			_home,
			_buffName
		}));

		std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(util::fs::path::build({
			_home,
			_buffName
		}));

		if (buff.size()) {
			_buff = buff;
		} else {
			_buff = {
				""
			};
		}
	} else {
		omni::err(omni::ERR_FS_NO_ENTRY, {
			fName
		});
	}

	if (util::fs::perm(util::fs::path::build({
		_home,
		_buffName
	}))[1] == 'w') {
		_cursEditor[MIN][X] = _buff.back().size() - 1 + 1;
		_cursEditor[MIN][Y] = _buff.size() - 1;
	} else {
		for (int i = 0; i < 2; i++) {
			_cursEditor[MIN][i] = 0;
		}
	}

	for (int i = 0; i < 2; i++) {
		_cursEditor[MAX][i] = _cursEditor[MIN][i];
	}

	addHist();
}

void Console::changeDir(std::string dir) {
	_cwd = util::fs::path::build(util::fs::path::entry(dir));

	_tree = util::fs::ls(util::fs::path::build({
		_home,
		_cwd
	}));

	if (_cwd != util::fs::path::curr) {
		_tree.push_back({
			{
				"name",
				util::fs::path::prev
			}, {
				"type",
				"dir"
			}
		});
	}

	std::sort(_tree.begin(), _tree.end(), lexicoEntry);

	_cursFs = 0;
}

void Console::switchMode(unsigned int mode) {
	_mode = mode;

	fmtScr();
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

	addHist();
}

void Console::newLine() {
	std::string tail = std::string(_buff[_cursEditor[MIN][Y]].begin() + _cursEditor[MIN][X], _buff[_cursEditor[MIN][Y]].end());

	_buff[_cursEditor[MIN][Y]].erase(_buff[_cursEditor[MIN][Y]].begin() + _cursEditor[MIN][X], _buff[_cursEditor[MIN][Y]].end());

	if (_cursEditor[MIN][Y] == _buff.size() - 1 && _cursEditor[MIN][X] == _buff[_cursEditor[MIN][Y]].size()) {
		_buff.push_back(tail);
	} else {
		_buff.insert(_buff.begin() + _cursEditor[MIN][Y] + 1, tail);
	}

	for (int i = 0; i < 2; i++) {
		_cursEditor[i][Y]++;
		_cursEditor[i][X] = 0;
	}

	_diff = true;

	addHist();
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
						// trailing line
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
				if (_buff[_cursEditor[_rngEditor][Y]].size()) { // non-blank line
					if (_cursEditor[_rngEditor][X] == _buff[_cursEditor[_rngEditor][Y]].size()) { // trailing
						_buff[_cursEditor[_rngEditor][Y]].pop_back();
					} else {
						_buff[_cursEditor[_rngEditor][Y]].erase(_buff[_cursEditor[_rngEditor][Y]].begin() + _cursEditor[MIN][X], _buff[_cursEditor[_rngEditor][Y]].begin() + _cursEditor[MAX][X] + 1);
					}
				} else {
					if (_buff.size() > 1) {
						_buff.erase(_buff.begin() + _cursEditor[_rngEditor][Y]);

						if (_cursEditor[_rngEditor][Y]) {
							_cursEditor[_rngEditor][Y]--;
						}

						console->_cursEditor[console->_rngEditor][X] = console->_buff[console->_cursEditor[console->_rngEditor][Y]].size();
					}
				}

				for (int r = 0; r < 2; r++) {
					if (_cursEditor[r][Y] > _buff.size()) {
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

				if (roof > _prompt.size()) {
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

			if (_cursPrompt[MIN] > _prompt.size()) {
				_cursPrompt[MIN] = _prompt.size();
			}
			_cursPrompt[MAX] = _cursPrompt[MIN];

			break;
	}

	addHist();
}

void Console::addHist() {
	if (_h < _hist.size() - 1) {
		_hist.erase(_hist.begin() + _h, _hist.end());
	}

	_hist.push_back(_buff);
	_h = _hist.size() - 1;
}

void Console::exec() {
	if (_prompt.size()) {
		std::vector<std::string> tok = util::str::split(_prompt, ' ');

		std::string cmd = tok[0];
		std::vector<std::string> arg = std::vector<std::string>(tok.begin() + 1, tok.end());

		if (omni::lib.find(cmd) != omni::lib.end()) {
			if (std::find(omni::lib[cmd].begin(), omni::lib[cmd].end(), arg.size()) != omni::lib[cmd].end()) {
				if (cmd == "open") {
					if (!_diff) {
						open(arg[0]);
					} else {
						omni::err(omni::ERR_BUFF_DIFF);
					}
				}

				if (cmd == "new") {
					if (!_diff) {
						_buffName = arg[0];
						_buff = {
							""
						};
					} else {
						omni::err(omni::ERR_BUFF_DIFF);
					}
				}

				if (cmd == "new_dir") {
					if (!util::fs::exist(arg[0])) {
						mkdir(arg[0].c_str(), S_IRWXU);
					} else {
						omni::err(omni::ERR_FS_DIR_EXIST);
					}

					_tree = util::fs::ls(util::fs::path::build({
						_home,
						_cwd
					}));
					if (_cwd != util::fs::path::curr) {
						_tree.push_back({
							{
								"name",
								util::fs::path::prev
							}, {
								"type",
								"dir"
							}
						});
					}

					std::sort(_tree.begin(), _tree.end(), lexicoEntry);
				}

				if (cmd == "save") {
					std::string fName;
					switch (arg.size()) {
						case 0:
							fName = _buffName;

							break;

						case 1:
							fName = arg[0];

							break;
					}

					bool refresh = !util::fs::exist(fName);

					util::fs::write(util::fs::path::build({
						_home,
						_cwd,
						fName
					}), _buff);

					if (refresh) {
						_tree = util::fs::ls(util::fs::path::build({
							_home,
							_cwd
						}));
						if (_cwd != util::fs::path::curr) {
							_tree.push_back({
								{
									"name",
									util::fs::path::prev
								}, {
									"type",
									"dir"
								}
							});
						}

						std::sort(_tree.begin(), _tree.end(), lexicoEntry);
					}

					_diff = false;
				}

				if (cmd == "del") {
					std::string fName;
					switch (arg.size()) {
						case 0:
							fName = _buffName;

							break;

						case 1:
							fName = arg[0];

							break;
					}

					util::fs::del(util::fs::path::build({
						_home,
						fName
					}));

					_tree = util::fs::ls(util::fs::path::build({
						_home,
						_cwd
					}));
					if (_cwd != util::fs::path::curr) {
						_tree.push_back({
							{
								"name",
								util::fs::path::prev
							}, {
								"type",
								"dir"
							}
						});
					}

					std::sort(_tree.begin(), _tree.end(), lexicoEntry);
				}

				if (cmd == "rename") {
					std::string fName;
					switch (arg.size()) {
						case 1:
							fName = _buffName;

							break;

						case 2:
							fName = arg[0];

							break;
					}

					if (util::fs::exist(util::fs::path::build({
						_home,
						_cwd,
						fName
					}))) {
						rename((util::fs::path::build({
							_home,
							_cwd,
							fName
						})).c_str(), (util::fs::path::build({
							_home,
							_cwd,
							arg.back()
						})).c_str());
					} else {
						omni::err(omni::ERR_FS_NO_ENTRY, {
							fName
						});
					}

					_tree = util::fs::ls(util::fs::path::build({
						_home,
						_cwd
					}));
					if (_cwd != util::fs::path::curr) {
						_tree.push_back({
							{
								"name",
								util::fs::path::prev
							}, {
								"type",
								"dir"
							}
						});
					}

					std::sort(_tree.begin(), _tree.end(), lexicoEntry);
				}

				if (cmd == "copy") {
					std::string name;
					switch (arg.size()) {
						case 1:
							name = _buffName;

							break;

						case 2:
							name = arg[0];

							break;
					}

					util::fs::cp(util::fs::path::build({
						_home,
						_cwd,
						name
					}), util::fs::path::build({
						_home,
						_cwd,
						arg.back()
					}));

					_tree = util::fs::ls(util::fs::path::build({
						_home,
						_cwd
					}));
					if (_cwd != util::fs::path::curr) {
						_tree.push_back({
							{
								"name",
								util::fs::path::prev
							}, {
								"type",
								"dir"
							}
						});
					}

					std::sort(_tree.begin(), _tree.end(), lexicoEntry);
				}

				if (cmd == "run") {
					if (!mtx) {
						std::string fName;

						if (arg.size()) {
							fName = arg[0];
						} else {
							fName = _buffName;
						}

						if (util::fs::exist(util::fs::path::build({
							_home,
							fName
						}))) {
							std::thread t(dispatch, util::fs::path::build({
								_home,
								fName
							}), maxFs() + maxNo());
							t.detach();
						} else {
							omni::err(omni::ERR_FS_NO_ENTRY, {
								util::fs::path::build({
									_home,
									fName
								})
							});
						}
					} else {
						omni::err(omni::ERR_SCRIPT_NOT_FIN);
					}
				}

				if (cmd == "set") {
					nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("stat.json"));

					unsigned int rank = deser["rank"];

					if (rank >= (int) omni::stage[stage]["val"]) {
						scn::init(arg[0], std::stoi(arg[1]));
					} else {
						omni::err(omni::ERR_RANK_LT, {
							arg[0]
						});
					}
				}

				if (cmd == "next") {
					if (eq) {
						adv = true;
					} else {
						omni::err(omni::ERR_LVL_NOT_FIN);
					}
				}

				if (cmd == "quit") {
					if (!_diff) {
						disp->open = false;
					} else {
						omni::err(omni::ERR_BUFF_DIFF);
					}
				}
			} else {
				omni::err(omni::ERR_ARG_CNT, {
					cmd
				});
			}
		} else {
			omni::err(omni::ERR_NO_CMD, {
				cmd
			});
		}
	}

	_prompt.clear();

	_cursPrompt[MIN] = _prompt.size();
	_cursPrompt[MAX] = _cursPrompt[MIN];

	fmtScr();
}

void Console::hl() {
	glBindTexture(GL_TEXTURE_2D, _tex);

	unsigned int loc[2] = {
		0
	};

	/* status bar */
	for (int i = 0; i < state::lineWd; i++) {
		unsigned int idx = util::math::idx::array({
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
	loc[X] += maxFs();
	loc[Y] = 1;

	/* editor */
	// line numbers
	if (state::hlLineNo) {
		int l = 0;
		int y = 0;
		while (l < boundFrame[Y]) {
			if (l < _buff.size()) {
				int c = 0;
				int x = 0;
				while (c < maxNo()) {
					unsigned int idx = util::math::idx::array({
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
			for (int c = 0; c < maxNo(); c++) {
				unsigned int idx = util::math::idx::array({
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
	const unsigned int boundEditor[2] = {
		boundFrame[X] - (maxFs() + maxNo()),
		boundFrame[Y]
	};

	switch (_mode) {
		case EDITOR: {
			loc[X] = maxFs() + maxNo();
			loc[Y] = 1;

			Coord start = {
				_cursEditor[MIN][X],
				_cursEditor[MIN][Y]
			};
			unsigned int startScal = util::math::idx::determ(start, _buff);

			Coord end = {
				_cursEditor[MAX][X],
				_cursEditor[MAX][Y]
			};
			unsigned int endScal = util::math::idx::determ(end, _buff);

			int delta = util::math::delta(startScal, endScal);

			int norm = util::math::norm(startScal, endScal);

			Coord idx = start;

			int i = 0;
			while (i <= abs(delta)) {
				_hl[util::math::idx::array({
					util::math::clamp<unsigned int>(loc[X] + idx.x, 1, boundFrame[X] - 1),
					util::math::clamp<unsigned int>(loc[Y] + idx.y, 1, boundFrame[Y] - 1)
				}, {
					state::lineWd,
					state::lineCnt
				})] = true;

				if (norm == 1) {
					if (idx.x < _buff[idx.y].size() - 1) { // line bounds
						idx.x++;
					} else {
						if (idx.y != end.y) { // wrap
							idx.y++;
							idx.x = 0;
						}
					}
				}

				if (norm == -1) {
					if (idx.x) { // initial character
						idx.x--;
					} else{
						if (idx.y) { // wrap
							idx.y--;
							idx.x = _buff[idx.y].size() - 1;
						}
					}
				}

				i++;
			}

			// block
			for (int r = 0; r < 2; r++) {
				if (_cursEditor[r][X] == _buff[_cursEditor[r][Y]].size()) {
					unsigned int clamped[2] = {
						util::math::clamp<unsigned int>(loc[X] + _cursEditor[r][X], 1, boundFrame[X] - 1),
						util::math::clamp<unsigned int>(loc[Y] + _cursEditor[r][Y], 1, boundFrame[Y])
					};

					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped[X] * layout::glyph[X], clamped[Y] * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_RGB, GL_UNSIGNED_BYTE, _block);
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
				unsigned int idx = util::math::idx::array({
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
				unsigned int clamped = util::math::clamp<unsigned int>(loc[X] + _cursPrompt[r], 1, state::lineWd - 1);

				if (_cursPrompt[r] == _prompt.size()) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, clamped * layout::glyph[X], (state::lineCnt - 1) * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_RGB, GL_UNSIGNED_BYTE, _block);
				}
			}

			break;
		}

		case FS: {
			loc[X] = 0;
			loc[Y] = 1;

			for (int c = 0; c < maxFs(); c++) {
				unsigned int idx = util::math::idx::array({
					loc[X] + c,
					loc[Y] + util::math::clamp<unsigned int>(_cursFs, 1, boundFrame[Y] - 1)
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
	Coord idx = util::math::coord::array(c, {
		16,
		16
	});

	unsigned int sz = layout::map[X] * layout::map[Y] * layout::glyph[X] * layout::glyph[Y];
	unsigned int stride[2] = {
		layout::glyph[X],
		layout::map[X] * layout::glyph[X] * layout::glyph[Y]
	};

	glTexSubImage2D(GL_TEXTURE_2D, 0, st.x * layout::glyph[X], st.y * layout::glyph[Y], layout::glyph[X], layout::glyph[Y], GL_BGR, GL_UNSIGNED_BYTE, &(((char*) _map->pixels)[((b * sz) + (idx.y * stride[Y]) + (idx.x * stride[X])) * 3]));
}

std::vector<std::string> Console::log() {
	unsigned int loc = _buff.size();
	unsigned int ptrEditorX = maxFs() + maxNo();

	std::vector<std::string> buff;

	std::string head = "Level complete";
	buff.push_back(head);

	std::string lb;
	for (int i = ptrEditorX; i < state::lineWd; i++) {
		lb.push_back('=');
	}
	buff.push_back(lb);

	std::map<std::string, std::string> attr = {
		{
			"LOC",
			std::to_string(loc)
		}, {
			"CALLS",
			std::to_string(call)
		}
	};

	for (std::map<std::string, std::string>::iterator it = attr.begin(); it != attr.end(); ++it) {
		std::string key = it->first;
		key += ":";

		std::string val = it->second;

		std::string pair = util::str::pad(key, (state::lineWd - ptrEditorX) - val.size());
		pair += val;

		buff.push_back(pair);
	}

	for (int i = buff.size(); i < state::lineCnt - 2 - 1 - 2 - 1; i++) {
		buff.push_back("");
	}

	buff.push_back("");
	buff.push_back(util::now(state::format));

	buff.push_back("");
	buff.push_back("Enter `next` to proceed");

	return buff;
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

	if (std::isdigit(lhs["name"][0]) && std::isdigit(rhs["name"][0])) {
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

std::vector<std::string> Console::slice(std::vector<std::string> buff, unsigned int rng[2][2]) {
	std::vector<std::string> _;

	std::string line;

	int delta = util::math::delta(util::math::idx::determ({
		rng[MIN][X],
		rng[MIN][Y]
	}, buff), util::math::idx::determ({
		rng[MAX][X],
		rng[MAX][Y]
	}, buff));

	unsigned int b = MIN;
	if (delta > 0) {
		b = MIN;
	}
	if (delta < 0) {
		b = MAX;
	}

	unsigned int start[2];
	for (int i = 0; i < 2; i++) {
		start[i] = rng[b][i];
	}

	int i = 0;
	unsigned int idx[2];
	for (int i = 0; i < 2; i++) {
		idx[i] = start[i];
	}
	while (i < 1 + abs(delta)) {
		i++;

		if (idx[X] < buff[idx[Y]].size()) {
			line.push_back(buff[idx[Y]][idx[X]]);

			idx[X]++;
		} else {
			_.push_back(line);
			line = "";

			idx[Y]++;
			idx[X] = 0;
		}
	}

	_.push_back(line);

	return _;
}
