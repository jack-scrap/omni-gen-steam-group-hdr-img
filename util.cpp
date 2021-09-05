#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <sstream>
#include <dirent.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sys/stat.h>

#include "util.h"
#include "math.h"
#include "state.h"
#include "scn.h"
#include "dict.h"
#include "line.h"
#include "omni.h"
#include "layout.h"

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
		omni::err("Could not open " + name + "; file not found");
	}

	std::vector<std::string> cont;
	for (std::string l; std::getline(in, l);) {
		cont.push_back(l);
	}

	in.close();

	return cont;
}

void util::fs::write(std::string name, std::vector<std::string> buff) {
	std::string stat = util::fs::perm(name);

	if (stat[1] != 'w') {
		omni::err("Could not write " + name + "; file read-only");

		return;
	}

	std::ofstream f;
	f.open(name);

	for (const std::string& line : buff) {
		f << line + '\n';
	}

	f.close();
}

void util::fs::del(std::string name) {
	if (remove(name.c_str()) != 0) {
		omni::err("Couldn't delete file `" + name + "`");
	}
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

std::string util::fs::perm(std::string name) {
	std::string _;

	struct stat f;
	if (stat(name.c_str(), &f) < 0) {
		return _;
	}

	_.push_back(f.st_mode & S_IRUSR ? 'r' : '-');
	_.push_back(f.st_mode & S_IWUSR ? 'w' : '-');
	_.push_back(f.st_mode & S_IXUSR ? 'x' : '-');

	return _;
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

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = util::str::split(buff[l], ' ');

		if (tok[0] == "v") {
			omni::assertion(tok.size() == 1 + 3, std::string("Could not load object `" ) + name + std::string("`; inappropriate size of vertex position at [" ) + std::to_string(l) + std::string("]"));

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

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = util::str::split(buff[l], ' ');

		if (tok[0] == "vt") {
			omni::assertion(tok.size() == 1 + 2, std::string("Could not load object `" ) + name + std::string("`; inappropriate size of texture coordinate at [" ) + std::to_string(l) + std::string("]"));

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

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = util::str::split(buff[l], ' ');

		if (tok[0] == "vn") {
			omni::assertion(tok.size() == 1 + 3, std::string("Could not load object `" ) + name + std::string("`; inappropriate size of normal at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + 3; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

void util::mesh::strip(GLushort idc[2][3]) {
	const int roof = (2 * 2) - 1;

	for (int t = 0; t < 2; t++) {
		for (int a = 0; a < 3; a++) {
			idc[t][a] = (t * roof) + ((t ? -1 : 1) * a);
		}
	}
}

void util::mesh::aabb(GLfloat bound[3][2], GLfloat* vtc, GLushort* idc, unsigned int noPrim) {
	for (int i = 0; i < noPrim; i++) {
		int idx = idc[i];

		for (int a = 0; a < 3; a++) {
			if (vtc[(idx * 3) + a] < bound[a][MIN]) {
				bound[a][MIN] = vtc[(idx * 3) + a];
			}

			if (vtc[(idx * 3) + a] > bound[a][MAX]) {
				bound[a][MAX] = vtc[(idx * 3) + a];
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
		for (int a = 0; a < 3; a++) {
			GLushort offset = (t * 3);

			_.push_back(offset + (a * (t ? -1 : 1)));
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::gen::norm(std::vector<glm::vec3> vtc) {
	std::vector<GLfloat> _;

	// get edges
	glm::vec3
		u = vtc[1] - vtc[0],
		v = vtc[2] - vtc[0];

		// calculate cross product
		glm::vec3 orth = glm::cross(u, v);

		// normalize
		glm::vec3 norm = glm::normalize(orth);

		for (int a = 0; a < 3; a++) {
			_.push_back(norm[a]);
		}

		return _;
}

std::vector<GLushort> util::mesh::rd::idc(std::string name, unsigned int attr) {
	std::vector<GLushort> _;

	std::vector<std::string> buff = util::fs::rd<std::vector<std::string>>("res/obj/" + name + ".obj");

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = util::str::split(buff[l], ' ');

		if (tok[0] == "f") {
			omni::assertion(tok.size() == 1 + 3, std::string("Could not load object `" ) + name + std::string("`; inappropriate number of indices for triangle primitive at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + 3; i++) {
				std::vector<std::string> type = util::str::split(tok[i], '/');

				_.push_back(std::stoi(type[attr]) - 1);
			}
		}
	}

	return _;
}

glm::mat4 util::matr::rot(glm::mat4 model, glm::vec3 rot) {
	glm::quat quat = glm::quat(rot);

	return model * glm::mat4_cast(quat);
}

glm::vec3 util::matr::apply(glm::vec3 vtx, glm::mat4 model) {
	return glm::vec3(model * glm::vec4(glm::vec3(0.0), 1.0));
}

bool util::phys::coll(Obj* p, Obj* q, glm::mat4 modelP, glm::mat4 modelQ) {
	bool _ = true;

	for (int a = 0; a < 3; a++) {
		glm::vec3
			vtxPMin = glm::vec3(0.0),
			vtxPMax = glm::vec3(0.0),
			vtxQMin = glm::vec3(0.0),
			vtxQMax = glm::vec3(0.0);

		vtxPMin[a] = p->_aabb[a][MIN];
		vtxPMin = glm::vec3(modelP * glm::vec4(vtxPMin, 1.0));

		vtxPMax[a] = p->_aabb[a][MAX];
		vtxPMax = glm::vec3(modelP * glm::vec4(vtxPMax, 1.0));

		vtxQMin[a] = p->_aabb[a][MIN];
		vtxQMin = glm::vec3(modelQ * glm::vec4(vtxQMin, 1.0));

		vtxQMax[a] = p->_aabb[a][MAX];
		vtxQMax = glm::vec3(modelQ * glm::vec4(vtxQMax, 1.0));

		if (!(
			vtxPMin[a] > vtxPMin[a] &&
			vtxPMin[a] < vtxPMax[a] &&
			vtxPMax[a] > vtxPMin[a] &&
			vtxPMax[a] < vtxPMax[a]
		)) {
			_ = false;

			break;
		}
	}

	return _;
}

bool util::phys::coll(glm::vec3 vtx, Obj* bound, glm::mat4 modelVtx, glm::mat4 modelBound) {
	bool _ = true;

	glm::vec3 vtxTrans = glm::vec3(modelVtx * glm::vec4(vtx, 1.0));

	for (int a = 0; a < 3; a++) {
		glm::vec3 rng[2];
		for (int i = 0; i < 2; i++) {
			rng[i] = glm::vec3(0.0);
		}

		rng[MIN][a] = bound->_aabb[a][MIN];
		rng[MIN] = glm::vec3(modelVtx * glm::vec4(rng[MIN], 1.0));

		rng[MAX][a] = bound->_aabb[a][MAX];
		rng[MAX] = glm::vec3(modelVtx * glm::vec4(rng[MAX], 1.0));

		if (!(
			vtx[a] > rng[MIN][a] &&
			vtx[a] < rng[MAX][a]
		)) {
			_ = false;

			break;
		}
	}

	return _;
}

bool util::phys::collGround(Obj* obj, glm::mat4 model) {
	glm::vec3 btm = util::matr::apply(glm::vec3(0.0, obj->_aabb[Y][MIN], 0.0), model);

	return btm[Y] < 0.0;
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
	for (const std::string& line : buff) {
		_ += line + '\n';
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

char* util::json::id(nlohmann::json deser) {
	std::string buff = deser.get<std::string>();

	unsigned int no = buff.size();
	char* _ = (char*) malloc((no + 1) * sizeof (char));
	for (int i = 0; i < no; i++) {
		_[i] = buff[i];
	}
	_[no] = '\0';
	
	return _;
}

char util::json::byte(nlohmann::json deser) {
	omni::assertion(util::json::ascii(deser), std::string("Data not ASCII applicable"));

	return (char) ((int) deser);
}

bool util::json::ascii(nlohmann::json deser) {
	return
		deser >= 0 &&
		deser <= 127;
}

CBuff util::json::array::dim(nlohmann::json deser, CBuff buff, int i) {
	switch (i) {
		case 0:
			buff._z = deser.size();

			break;

		case 1:
			buff._y = deser.size();

			break;

		case 2:
			buff._x = deser.size();

			break;
	}

	if (deser[0].type() == nlohmann::json::value_t::array) {
		return dim(deser[0], buff, i + 1);
	} else {
		return buff;
	}
}

bool util::json::array::euclid(nlohmann::json deser, unsigned int sz) {
	bool _;

	if (sz > 3) {
		_ = false;
	} else {
		if (deser.type() == nlohmann::json::value_t::array) {
			_ = euclid(deser[0], sz + 1);
		} else {
			_ = true;
		}
	}

	return _;
}

CBuff util::json::str(nlohmann::json deser) {
	CBuff _;

	_._y = 1;
	_._z = 1;

	std::string buff = deser;

	_._x = buff.size();

		_._ptr = (char*) malloc(_._x * sizeof (char));

	for (int i = 0; i < buff.size(); i++) {
		((char*) _._ptr)[i] = buff[i];
	}

	return _;
}

CBuff util::json::array::array(nlohmann::json deser) {
	CBuff _;

	_._x = deser.size();
	_._y = 1;
	_._z = 1;
	_._ptr = (char*) malloc(_._x * sizeof (char));
	int c = 0;
	for (int i = 0; i < _._x; i++) {
		omni::assertion(util::json::ascii(deser[i]), std::string("Data at index [") + std::to_string(i) + std::string("] not ASCII applicable"));

		((char*) _._ptr)[c] = (char) ((int) deser[i]);

		c++;
	}

	return _;
}

CBuff util::json::array::matr(nlohmann::json deser) {
	CBuff _;

	_._x = deser[0].size();
	_._y = deser.size();
	_._z = 1;
	_._ptr = (char*) malloc(_._x * _._y * sizeof (char));
	int c = 0;
	for (int j = 0; j < _._y; j++) {
		for (int i = 0; i < _._x; i++) {
			omni::assertion(util::json::ascii(deser[j][i]), std::string("Data at index [") + std::to_string(j) + "][" + std::to_string(i) + std::string("] not ASCII applicable"));

			((char*) _._ptr)[c] = (char) ((int) deser[j][i]);

			c++;
		}
	}

	return _;
}

CBuff util::json::array::tens(nlohmann::json deser) {
	CBuff _;

	_._x = deser[0][0].size();
	_._y = deser[0].size();
	_._z = deser.size();
	_._ptr = (char*) malloc(_._x * _._y * _._x * sizeof (char));
	int c = 0;
	for (int k = 0; k < _._y; k++) {
		for (int j = 0; j < _._y; j++) {
			for (int i = 0; i < _._x; i++) {
				omni::assertion(util::json::ascii(deser[k][j][i]), std::string("Data at index [") + std::to_string(j) + "][" + std::to_string(i) + std::string("] not ASCII applicable"));

				((char*) _._ptr)[c] = (char) ((int) deser[k][j][i]);

				c++;
			}
		}
	}

	return _;
}

Var* util::json::var(nlohmann::json key, nlohmann::json val) {
	Var* _;

	char* id = util::json::id(key);

	glm::vec3 loc = glm::vec3(0.0);
	if (val.contains("loc")) {
		loc = util::json::vec(val["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (val.contains("rot")) {
		rot = util::json::vec(val["rot"]);
	}

	switch (val["block"].type()) {
		// scalar
		case nlohmann::json::value_t::number_unsigned: {
			char init = util::json::byte(val["block"]);

			Idx* idx = idxMk(0, &init, 1, key, loc, rot);

			omni::assertion(!(util::phys::collGround(idx->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			_ = varMk(id, idx);

			break;
		}

		// array
		case nlohmann::json::value_t::array: {
			switch (val["block"][0].type()) {
				// 1D
				case nlohmann::json::value_t::number_unsigned: {
					CBuff init = util::json::array::array(val["block"]);

					Array* val = arrayMk((char*) init._ptr, init._x, key, X, loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

					omni::assertion(!(util::phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

					_ = varMk(id, val);

					break;
				}

				// matrix
				case nlohmann::json::value_t::array: {
					switch (val["block"][0][0].type()) {
						// 2D
						case nlohmann::json::value_t::number_unsigned: {
							CBuff init = util::json::array::matr(val["block"]);

							Array* val = arrayMk((char*) init._ptr, init._x, init._y, key, loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

							omni::assertion(!(util::phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

							_ = varMk(id, val);

							break;
						}

						// 3D
						case nlohmann::json::value_t::array: {
							CBuff init = util::json::array::tens(val["block"]);

							Array* val = arrayMk((char*) init._ptr, init._x, init._y, key, loc, rot);

							omni::assertion(!(util::phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

							_ = varMk(id, val);

							break;
						}
					}
				}
			}

			break;
		}

		// string
		case nlohmann::json::value_t::string: {
			CBuff init = util::json::str(val["block"]);

			Array* val = arrayMk((char*) init._ptr, init._x, key, X, loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

			omni::assertion(!(util::phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			_ = varMk(id, val);

			break;
		}
	}

	return _;
}

Scope util::json::scope(nlohmann::json deser) {
	Scope _;

	_._no = deser.size();
	_._ptr = (Var**) malloc(_._no * sizeof (Var*));
	_._type = (unsigned int*) malloc(_._no * sizeof (unsigned int));

	unsigned int i = 0;
	for (const auto& pair : deser.items()) {
		char* id = util::json::id(pair.key());

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
				Var* var = util::json::var(pair.key(), pair.value());

				((Var**) _._ptr)[i] = var;
				((unsigned int*) _._type)[i] = omni::SCALAR;
				i++;
	
				break;
			}

			// string
			case nlohmann::json::value_t::string: {
				Var* var = util::json::var(pair.key(), pair.value());

				((Var**) _._ptr)[i] = var;
				((unsigned int*) _._type)[i] = omni::ARRAY;
				i++;

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				omni::assertion(util::json::array::euclid(pair.value()["block"], 0), std::string("Depth of `") + pair.key() + std::string("` exceeds 3 dimensions"));

				switch (pair.value()["block"][0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						Var* var = util::json::var(pair.key(), pair.value());

						((Var**) _._ptr)[i] = var;
						((unsigned int*) _._type)[i] = omni::ARRAY;
						i++;

						break;
					}

					// matrix
					case nlohmann::json::value_t::array: {
						switch (pair.value()["block"][0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								Var* var = util::json::var(pair.key(), pair.value());

								((Var**) _._ptr)[i] = var;
								((unsigned int*) _._type)[i] = omni::ARRAY;
								i++;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								Var* var = util::json::var(pair.key(), pair.value());

								((Var**) _._ptr)[i] = var;
								((unsigned int*) _._type)[i] = omni::ARRAY;
								i++;

								break;
							}
						}

						break;
					}
				}

				break;
			}
		}

		i++;
	}

	return _;
}

glm::vec3 util::json::vec(nlohmann::json deser) {
	glm::vec3 _;

	for (int a = 0; a < 3; a++) {
		_[a] = deser[a];
	}

	return _;
}

StreetSign* util::json::streetSign(nlohmann::json deser) {
	std::vector<bool> pass = util::json::ls<bool>(deser["pass"]);

	unsigned int no = pass.size();
	bool* array = (bool*) malloc(no * sizeof (bool));
	for (int i = 0; i < no; i++) {
		array[i] = pass[i];
	}

	glm::vec3 loc = glm::vec3(0.0);
	if (deser.contains("loc")) {
		loc = util::json::vec(deser["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (deser.contains("rot")) {
		rot = util::json::vec(deser["rot"]);
	}

	StreetSign* _ = streetSignMk({
		array,
		no
	}, loc, rot);

	omni::assertion(!util::phys::collGround(_->_parent), "Street sign clipping into ground plane");

	return _;
}

std::vector<Obj*> util::json::path(nlohmann::json path, nlohmann::json node, nlohmann::json status) {
	std::vector<Obj*> _;

	GLushort idc[2] = {
		0, 1
	};

	std::string name;
	bool active;
	if (status == "pass") {
		name = "solid";
		active = false;
	}

	if (status == "halt") {
		name = "solid";
		active = true;
	}

	if (status == "alert") {
		name = "alertRoad";
		active = true;
	}

	for (int i = 0; i < path.size() - 1; i++) {
		switch (path[1].type()) {
			case nlohmann::json::value_t::number_unsigned: {
				GLushort pt[2] = {
					path[i],
					path[i + 1]
				};

				GLfloat vtc[2][3];
				for (int p = 0; p < 2; p++) {
					for (int a = 0; a < 3; a++) {
						vtc[p][a] = node[pt[p]][a];
					}
				}

				_.push_back(lineMk((GLfloat*) vtc, idc, 2, "main", "lane", name, active));

				break;
			}

			case nlohmann::json::value_t::array: {
				for (const auto& no : path[1]) {
					GLushort pt[2] = {
						path[i],
						no
					};

					GLfloat vtc[2][3];
					for (int p = 0; p < 2; p++) {
						for (int a = 0; a < 3; a++) {
							vtc[p][a] = node[pt[p]][a];
						}
					}

					_.push_back(lineMk((GLfloat*) vtc, idc, 2, "obj", "thick", name, active));
				}

				break;
			}
		}
	}

	return _;
}

template <typename T>
std::vector<T> util::json::ls(nlohmann::json deser) {
	std::vector<T> _;

	for (int i = 0; i < deser.size(); i++) {
		_.push_back((T) deser[i]);
	}

	return _;
}

Obj* util::json::prop(nlohmann::json deser) {
	glm::vec3 loc = glm::vec3(0.0);
	if (deser.contains("loc")) {
		loc = util::json::vec(deser["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (deser.contains("rot")) {
		rot = util::json::vec(deser["rot"]);
	}

	Obj* _ = objMk(deser["name"], "obj", "dir", false, loc, rot);

	omni::assertion(!util::phys::collGround(_), std::string("Prop `") + std::string(deser["name"]) + std::string("` clipping into ground plane"));

	return _;
}

Lim* util::json::bound::lim(nlohmann::json val) {
	unsigned int axis;
	if (val["axis"] == "X") {
		axis = X;
	}

	if (val["axis"] == "Z") {
		axis = Z;
	}

	unsigned int status;
	if (val["status"] == "pass") {
		status = Lim::PASS;
	}

	if (val["status"] == "halt") {
		status = Lim::HALT;
	}

	if (val["status"] == "alert") {
		status = Lim::ALERT;
	}

	Lim* _ = limMk(axis, val["val"], status);

	omni::assertion(!util::phys::collGround(_->_parent), "Limit clipping into ground plane");

	return _;
}

Cone* util::json::bound::area(nlohmann::json deser) {
	GLfloat init[2][2];
	for (int a = 0; a < 2; a++) {
		for (int b = 0; b < 2; b++) {
			init[a][b] = deser["bound"][a][b];
		}
	}

	glm::vec3 loc = glm::vec3(0.0);
	if (deser.contains("loc")) {
		loc = util::json::vec(deser["loc"]);
	}

	Cone* _ = coneMk(init, loc);

	omni::assertion(!util::phys::collGround(_->_parent), "Cone clipping into ground plane");

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

bool util::cfg::boolean(std::string buff) {
	bool _ = true;

	if (buff.size() != 1) {
		_ = false;
	}

	if (
		buff[0] != 'n' ||
		buff[0] != 'y'
	) {
		_ = false;
	}

	return _;
}

std::map<std::string, std::string> util::cfg::lex(std::string name) {
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

int util::math::delta(int min, int max) {
	return max - min;
}

int util::math::norm(int lhs, int rhs) {
	int _ = 0;

	int delta = rhs - lhs;
	if (delta > 0) {
		_ = 1;
	}
	if (delta < 0) {
		_ = -1;
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

GLuint util::tex::spray(Obj* obj) {
	/* framebuffer */
	GLuint fbo;
	glGenFramebuffers(1, &fbo);

	// color
	GLuint cbo;
	glGenTextures(1, &cbo);
	glBindTexture(GL_TEXTURE_2D, cbo);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1000, 1000, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// render (stencil)
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

	objDraw(obj, Mesh::OBJ);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return cbo;
}
