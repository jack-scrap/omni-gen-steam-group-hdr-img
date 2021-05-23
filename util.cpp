#include <iostream>
#include <sstream>
#include <dirent.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sys/stat.h>

#include "util.h"
#include "math.h"
#include "state.h"
#include "scn.h"
#include "dict.h"
#include "omni.h"

template <typename T>
T util::fs::rd(std::string name) {
}

template <>
std::string util::fs::rd<std::string>(std::string name) {
	std::ifstream in;
	in.open(name);

	if (in.fail()) {
		omni::err("Could not open " + name + "; file not found");
	}

	std::string cont;
	for (std::string l; std::getline(in, l);) {
		cont += l + "\n";
	}

	in.close();

	return cont;
}

template <>
std::vector<std::string> util::fs::rd<std::vector<std::string>>(std::string name) {
	std::ifstream in;
	in.open("./" + name);

	if (in.fail()) {
		omni::err("Couldn't open " + name + "; file not found");
	}

	std::vector<std::string> cont;
	for (std::string l; std::getline(in, l);) {
		cont.push_back(l);
	}

	in.close();

	return cont;
}

void util::fs::write(std::string name, std::vector<std::string> buff) {
	bool w = util::fs::w(name);

	if (!w) {
		omni::err("Could not write " + name + "; file read-only");

		return;
	}

	std::ofstream f;
	f.open(name);

	for (const std::string& l : buff) {
		f << l + '\n';
	}

	f.close();
}

std::vector<std::map<std::string, std::string>> util::fs::ls(std::string name) {
	std::vector<std::map<std::string, std::string>> tree;

	std::string path = "./" + name;
	auto dir = opendir(path.c_str());

	if (!dir) {
		omni::err("Could not open directory " + path);
	}

	while (auto entity = readdir(dir)) {
		if (strcmp(entity->d_name, ".")) {
			std::string type;
			if (entity->d_type == DT_REG) {
				type = "file";
			}

			if (entity->d_type == DT_DIR) {
				type = "dir";
			}

			if (strcmp(entity->d_name, "..")) {
				tree.push_back({
					{
						"name",
						std::string(entity->d_name)
					}, {
						"type",
						type
					}
				});
			} else {
				if (name != ".") {
					tree.push_back({
						{
							"name",
							std::string(entity->d_name)
						}, {
							"type",
							type
						}
					});
				}
			}
		}
	}

	closedir(dir);

	return tree;
}

std::string util::fs::name(std::string buff) {
	std::vector<std::string> name = util::str::split(buff, '/');

	std::string f = name[name.size() - 1];

	return f;
}

std::string util::fs::base(std::string buff) {
	std::string f = util::fs::name(buff);

	return util::str::split(f, '.')[0];
}

bool util::fs::w(std::string name) {
	struct stat info;
	stat(name.c_str(), &info);
	int perm = info.st_mode & S_IWUSR;

	return perm == 128;
}

std::vector<GLfloat> util::mesh::plane(glm::vec2 sz) {
	std::vector<GLfloat> _;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_.push_back(x * sz[X]);
			_.push_back(y * sz[Y]);
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::rd::vtc(std::string name) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "v") {
			for (int i = 1; i < 1 + 3; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::rd::st(std::string name) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "vt") {
			for (int i = 1; i < 1 + 2; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::rd::norm(std::string name) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "vn") {
			for (int i = 1; i < 1 + 3; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

std::vector<GLushort> util::mesh::strip(std::vector<GLushort> idc) {
	std::vector<GLushort> _;

	bool far = false;

	for (int t = 0; t < 2; t++) {
		for (int i = 0; i < 3; i++) {
			int
				start = (far * 2) + (2 * t),
				step = (i * (t ? -1 : 1)),
				last = (3 * (t && i == 2));

			_.push_back(start + step + last);
		}
	}

	return _;
}

void util::mesh::bound(GLfloat rng[3][2], GLfloat* vtc, unsigned int noVtc) {
	for (int v = 0; v < noVtc; v += 3) {
		for (int i = 0; i < 3; i++) {
			if (vtc[v + i] < rng[i][MIN]) {
				rng[i][MIN] = vtc[v + i];
			}

			if (vtc[v + i] > rng[i][MAX]) {
				rng[i][MAX] = vtc[v + i];
			}
		}
	}
}

std::vector<GLfloat> util::mesh::rect::pos(glm::vec2 sz, unsigned int up, bool norm) {
	std::vector<GLfloat> _;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_.push_back((x ? 1 : (norm ? -1 : 0)) * sz[X]);

			if (up == Y) {
				_.push_back((y ? 1 : (norm ? -1 : 0)) * sz[Y]);
				_.push_back(0.0);
			}

			if (up == Z) {
				_.push_back(0.0);
				_.push_back((y ? 1 : (norm ? -1 : 0)) * sz[Y]);
			}
		}
	}

	return _;
}

std::vector<GLushort> util::mesh::rect::idc() {
	std::vector<GLushort> _;

	for (int t = 0; t < 2; t++) {
		for (int i = 0; i < 3; i++) {
			GLushort offset = (t * 3);

			_.push_back(offset + (i * (t ? -1 : 1)));
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::gen::normTri(std::vector<glm::vec3> vtc) {
	std::vector<GLfloat> _;

	// get edges
	glm::vec3
		u = vtc[1] - vtc[0],
		v = vtc[2] - vtc[0];

		// calculate cross product
		glm::vec3 orth = glm::cross(u, v);

		// normalize
		glm::vec3 norm = glm::normalize(orth);

		for (int i = 0; i < 3; i++) {
			_.push_back(norm[i]);
		}

		return _;
}

std::vector<GLushort> util::mesh::rd::idc(std::string name, unsigned int attr) {
	std::vector<GLushort> obj;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (const std::string& l : buff) {
		std::vector<std::string> tok = util::str::split(l, ' ');

		if (tok[0] == "f") {
			for (int i = 1; i < 1 + 3; i++) {
				std::vector<std::string> type = util::str::split(tok[i], '/');

				obj.push_back(std::stoi(type[attr]) - 1);
			}
		}
	}

	return obj;
}

glm::mat4 util::matr::rot(glm::mat4 model, glm::vec3 rot) {
	glm::quat quat = glm::quat(rot);

	return model * glm::mat4_cast(quat);
}

glm::vec3 apply(glm::vec3 vtx, glm::mat4 model) {
	return glm::vec3(model * glm::vec4(glm::vec3(0.0), 1.0));
}

bool util::phys::aabb(Obj* p, Obj* q) {
	bool _ = false;

	return _;
}

bool util::phys::aabbGround(Obj* obj) {
	bool _ = false;

	return _;
}

std::vector<std::string> util::str::split(std::string buff, char delim) {
	std::vector<std::string> tok;

	std::stringstream s(buff);
	std::string seg;
	while (std::getline(s, seg, delim)) {
		tok.push_back(seg);
	}

	return tok;
}

std::string util::str::join(std::vector<std::string> buff) {
	std::string _;
	for (const std::string& l : buff) {
		_ += l + '\n';
	}

	return _;
}

std::string util::str::pad(std::string buff, unsigned int roof) {
	std::string line = buff;
	for (int i = buff.length(); i < roof; i++) {
		line.push_back(' ');
	}

	return line;
}

char* util::json::id(nlohmann::json serial) {
	std::string buff = serial.get<std::string>();

	unsigned int no = buff.size();
	char* _ = (char*) malloc((no + 1) * sizeof (char));
	for (int i = 0; i < no; i++) {
		_[i] = buff[i];
	}
	_[no] = '\0';
	
	return _;
}

char util::json::byte(nlohmann::json serial) {
	return (char) ((int) serial);
}

cBuff util::json::arr(nlohmann::json serial) {
	cBuff _;

	_._x = serial.size();
	_._y = 1;
	_._z = 1;
	_._ptr = (char*) malloc(_._x * sizeof (char));
	for (int i = 0; i < _._x; i++) {
		((char*) _._ptr)[i] = (char) ((int) serial[i]);
	}

	return _;
}

cBuff util::json::matr2(nlohmann::json serial) {
	cBuff _;

	_._x = serial[0].size();
	_._y = serial.size();
	_._z = 1;
	_._ptr = (char*) malloc(_._x * _._y * sizeof (char));
	for (int j = 0; j < _._y; j++) {
		for (int i = 0; i < _._x; i++) {
			((char*) _._ptr)[(j * _._x) + i] = (char) ((int) serial[j][i]);
		}
	}

	return _;
}

cBuff util::json::matr3(nlohmann::json serial) {
	cBuff _;

	_._x = serial[0][0].size();
	_._y = serial[0].size();
	_._z = serial.size();

	_._ptr = (char*) malloc(0);

	return _;
}

glm::vec3 util::json::vec(nlohmann::json serial) {
	glm::vec3 _;

	for (int i = 0; i < 3; i++) {
		_[i] = serial[i];
	}

	return _;
}

void util::json::scope(nlohmann::json serial, Var**& data, unsigned int*& type, std::vector<Obj*>& obj) {
	data = (Var**) malloc(serial.size() * sizeof (Var*));
	type = (unsigned int*) malloc(serial.size() * sizeof (unsigned int*));

	unsigned int i = 0;
	for (const auto& pair : serial.items()) {
		glm::vec3 loc = glm::vec3(0.0);
		if (pair.value().contains("loc")) {
			loc = util::json::vec(pair.value()["loc"]);
		}

		glm::vec3 rot = glm::vec3(0.0);
		if (pair.value().contains("rot")) {
			rot = util::json::vec(pair.value()["rot"]);
		}

		switch (pair.value()["block"].type()) {
			// scalar
			case nlohmann::json::value_t::number_unsigned: {
				char init = util::json::byte(pair.value()["block"]);

				Idx* val = idxMk(0, &init, 1, pair.key(), loc, rot);

				for (const auto& pair : serial.items()) {
					char* id = util::json::id(pair.key());

					Var* _ = varMk(id, val);

					data[i] = _;
					type[i] = SCALAR;
					i++;

					obj.push_back(val->_parent);
				}

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				for (const auto& pair : serial.items()) {
					glm::vec3 loc = glm::vec3(0.0);
					if (pair.value().contains("loc")) {
						loc = util::json::vec(pair.value()["loc"]);
					}

					glm::vec3 rot = glm::vec3(0.0);
					if (pair.value().contains("rot")) {
						rot = util::json::vec(pair.value()["rot"]);
					}

					switch (pair.value()["block"][0].type()) {
						// 1D
						case nlohmann::json::value_t::number_unsigned: {
							cBuff init = util::json::arr(pair.value()["block"]);

							Arr* val = arrMk((char*) init._ptr, init._x, pair.key(), loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

							char* id = util::json::id(pair.key());

							Var* _ = varMk(id, val);

							data[i] = _;
							type[i] = ARRAY;
							i++;

							obj.push_back(((Arr*) (((Var*) data[i - 1])->_ptr))->_parent);

							break;
						}

						// matrix
						case nlohmann::json::value_t::array: {
							switch (pair.value()["block"][0][0].type()) {
								case nlohmann::json::value_t::number_unsigned: {
									cBuff init = util::json::matr2(pair.value()["block"]);

									char* id = util::json::id(pair.key());

									Arr* val = arrMk((char*) init._ptr, init._x, init._y, pair.key(), loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

									i++;
									Var* _ = varMk(id, val);
									data[i - 1] = _;

									obj.push_back(((Arr*) (((Var*) data[i - 1])->_ptr))->_parent);

									break;
								}

								case nlohmann::json::value_t::array: {
									cBuff init = util::json::matr3(pair.value()["block"]);

									char* id = util::json::id(pair.key());

									Arr* val = arrMk((char*) init._ptr, init._x, init._y, init._z, pair.key(), loc, rot);

									Var* _ = varMk(id, val);

									i++;
									data = (Var**) realloc(data, i * sizeof (void*));
									type = (unsigned int*) realloc(type, i * sizeof (unsigned int*));
									data[i - 1] = _;
									type[i - 1] = ARRAY;

									obj.push_back(val->_parent);

									break;
								}
							}

							break;
						}
					}
				}

				break;
			}

			// dictionary
			case nlohmann::json::value_t::object: {
				unsigned int no = pair.value()["block"].size();
				void** data = (void**) malloc(no * sizeof (void*));
				unsigned int* type = (unsigned int*) malloc(no * sizeof (unsigned int));

				for (const auto& pair : pair.value()["block"].items()) {
					void* _;
					unsigned int t;

					// scalar
					switch (pair.value()["block"].type()) {
						case nlohmann::json::value_t::number_unsigned: {
							char* init = (char*) malloc(sizeof (char));
							init[0] = (char) ((int) pair.value()["block"]);

							_ = idxMk(0, init, 1);
							t = SCALAR;

							break;
						}
					}

					data[no] = _;
					type[no] = t;
					no++;
				}	

				Dict* _ = dictMk(data, type, no, "", loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

				obj.push_back(_->_parent);

				break;
			}
		}

		i++;
	}
}

void util::json::prop(nlohmann::json serial, std::vector<Obj*>& obj) {
	glm::vec3 loc = glm::vec3(0.0);
	if (serial.contains("loc")) {
		loc = util::json::vec(serial["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (serial.contains("rot")) {
		rot = util::json::vec(serial["rot"]);
	}

	Obj* _ = objMk(serial["name"], "obj", "dir", false, loc, rot);

	obj.push_back(_);
}

void util::json::bound::rng(nlohmann::json serial) {
	for (const auto& rng : serial) {
		for (const auto& lim : rng) {
			for (const auto& pair : lim.items()) {
				unsigned int axis;
				if (pair.key() == "X") {
					axis = X;
				}

				if (pair.key() == "Z") {
					axis = Z;
				}

				Lim* _ = limMk(axis, pair.value());

				boundRng[noBoundRng] = _;
				noBoundRng++;

				line.push_back(_->_parent);
			}
		}
	}
}

Cone* util::json::bound::area(nlohmann::json serial) {
	GLfloat init[2][2];
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			init[y][x] = serial["init"][y][x];
		}
	}

	glm::vec3 loc = glm::vec3(0.0);
	if (serial.contains("loc")) {
		loc = util::json::vec(serial["loc"]);
	}

	Cone* _ = coneMk(init, loc);

	boundArea[noBoundArea] = _;
	noBoundArea++;

	obj.push_back(_->_parent);

	return _;
}

std::string util::cfg::key(std::string buff) {
	std::string _;

	int i = 0;
	while (i < buff.size()) {
		if (buff[i] == '[') {
			i++;

			while (buff[i] != ']') {
				_.push_back(buff[i]);

				i++;
			}
		}

		i++;
	}

	return _;
}

bool util::cfg::var(std::string buff) {
	bool _ = true;

	for (int i = 0; i < buff.size(); i++) {
		if (
			!isdigit(buff[i]) &&
			!isalpha(buff[i]) &&
			buff[i] != '_'
		) {
			_ = false;

			break;
		}
	}

	return _;
}

bool util::cfg::no(std::string buff) {
	bool _ = true;

	for (int i = 0; i < buff.size(); i++) {
		if (!isdigit(buff[i])) {
			_ = false;

			break;
		}
	}

	return _;
}

template <typename T>
std::map<std::string, T> util::cfg::parse(std::string name) {
}

template <>
std::map<std::string, std::string> util::cfg::parse<std::string>(std::string name) {
	std::map<std::string, std::string> _;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>(name);
	for (const std::string& line : buff) {
		std::vector<std::string> ast;

		// lex
		int i = 0;
		while (i < line.size()) {
			// whitespace
			if (isspace(line[i])) {
				i++;

				continue;
			}

			// string
			if (line[i] == '\'') {
				std::string tok;

				// opening quote
				i++;

				while (
					line[i] != '\'' &&
					i < line.size()
				) {
					tok.push_back(line[i]);

					i++;
				}

				// closing quote
				i++;

				ast.push_back(tok);

				continue;
			}

			// token
			if (!isspace(line[i])) {
				std::string tok;

				while (
					!isspace(line[i]) &&
					i < line.size()
				) {
					tok.push_back(line[i]);

					i++;
				}

				ast.push_back(tok);

				continue;
			}
		}

		if (ast.size()) {
			// parse
			if (ast.size() == 1) {
				continue;
			}

			// error
			if (ast.size() != 3) {
				omni::err("Inappropriate number of tokens in config entry `" + ast[0] + "`");
			}

			if (ast[1] != "=") {
				omni::err("Inappropriate token `" + ast[1] + "`");
			}

			if (!cfg::var(ast[0])) {
				omni::err("Inappropriate key `" + ast[0] + "`, can only be alpha-numeric with `_` separator");

				break;
			}

			_[ast[0]] = ast[2];
		}
	}

	return _;
}

std::vector<std::string> util::log(unsigned int loc, unsigned int maxFs) {
	std::vector<std::string> buff;

	unsigned int maxNo = std::to_string(state::line).size();

	std::string head = "Level complete";
	buff.push_back(head);

	std::string lb;
	for (int i = 0; i < state::ln; i++) {
		lb.push_back('=');
	}
	buff.push_back(lb);

	std::string key = "LOC: ";
	std::string val = std::to_string(loc);

	std::string pair = util::str::pad(key, state::ln - (val.size() + maxFs + 1 + maxNo + 1));
	pair += val;

	buff.push_back(pair);

	for (int i = buff.size(); i < state::line - 2 - 1 - 2 - 1; i++) {
		buff.push_back("");
	}

	buff.push_back("");
	buff.push_back(util::now(state::format));

	buff.push_back("");
	buff.push_back("Enter `next` to proceed");

	return buff;
}

std::string util::now(std::string format) {
	time_t epoch;
	time(&epoch);

	struct tm* info = localtime(&epoch);

	char out[state::ln];
	strftime(out, sizeof out / sizeof *out, format.c_str(), info);

	return std::string(out);
}

GLuint util::tex::spray(char c) {
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
	std::vector<GLushort> idc = util::mesh::rd::idc(std::string("glyph/") + c, 0);
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

	/* framebuffer */
	GLuint fbo;
	glGenFramebuffers(1, &fbo);

	// color
	GLuint cbo;
	glGenTextures(1, &cbo);
	glBindTexture(GL_TEXTURE_2D, cbo);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1000, 1000, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// renderbuffer (stencil)
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 800, 600);

	// attach texture, renderbuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		omni::err("Framebuffer not complete");
	}

	// draw
	prog.unUse();

	disp->clear();

	glBindVertexArray(vao);
	prog.use();

	glDrawElements(GL_TRIANGLES, idc.size(), GL_UNSIGNED_SHORT, (GLvoid*) 0);

	prog.unUse();
	glBindVertexArray(0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return cbo;
}
