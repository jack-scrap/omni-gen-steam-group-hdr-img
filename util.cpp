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
T util::fs::rd(std::string fName) {
}

template <>
std::string util::fs::rd<std::string>(std::string fName) {
	std::ifstream in;
	in.open(fName);

	std::string cont;

	if (exist(fName)) {
		for (std::string l; std::getline(in, l);) {
			cont += l + "\n";
		}
	} else {
		omni::err(omni::ERR_FS_NO_ENTRY, {
			path::build(path::entry(fName))
		});
	}

	in.close();

	return cont;
}

template <>
std::vector<std::string> util::fs::rd<std::vector<std::string>>(std::string fName) {
	std::ifstream in;
	in.open(path::build({
		path::curr,
		fName
	}));

	std::vector<std::string> cont;

	if (exist(fName)) {
		for (std::string l; std::getline(in, l);) {
			cont.push_back(l);
		}
	} else {
		omni::err(omni::ERR_FS_NO_ENTRY, {
			path::build(path::entry(fName))
		});
	}

	in.close();

	return cont;
}

bool util::fs::exist(std::string name) {
	struct stat info;

	return !stat(name.c_str(), &info);
}

void util::fs::write(std::string fName, std::vector<std::string> buff) {
	if (exist(fName)) {
		std::string stat = perm(fName);

		if (stat[1] != 'w') {
			omni::err(omni::ERR_FS_RO, {
				fName
			});

			return;
		}
	}

	std::vector<std::string> entry = path::entry(fName);
	entry.insert(entry.begin(), path::curr);

	std::string f = entry.back();

	entry.pop_back();

	if (exist(path::build(entry))) {
		std::ofstream f;
		f.open(fName);

		for (const std::string& line : buff) {
			f << line + '\n';
		}

		f.close();
	} else {
		omni::err(omni::ERR_FS_NO_PARENT_DIR, {
			f,
			entry.back()
		});
	}
}

void util::fs::del(std::string name) {
	struct stat s;
	stat(name.c_str(), &s);

	if (s.st_mode & S_IFDIR) {
		std::vector<std::map<std::string, std::string>> tree = ls(name);
		for (int i = 0; i < tree.size(); i++) {
			if (tree[i]["name"] == path::prev) {
				tree.erase(tree.begin() + i);
			}
		}

		// entries
		for (int i = 0; i < tree.size(); i++) {
			if (s.st_mode & S_IFDIR) {
				del(path::build({
					name,
					tree[i]["name"]
				}));
			} else {
				if (remove(path::build({
					name,
					tree[i]["name"]
				}).c_str())) {
					omni::err(omni::ERR_FS_DEL_ENTRY, {
						path::build({
							name,
							tree[i]["name"]
						})
					});
				}
			}
		}

		// empty directory
		if (remove(name.c_str())) {
			omni::err(omni::ERR_FS_DEL_ENTRY, {
				name
			});
		}
	} else {
		if (remove(name.c_str())) {
			omni::err(omni::ERR_FS_DEL_ENTRY, {
				name
			});
		}
	}
}

void util::fs::cp(std::string src, std::string dest) {
	struct stat s;
	stat(src.c_str(), &s);

	if (exist(src)) {
		if (s.st_mode & S_IFREG) {
			std::vector<std::string> buff = rd<std::vector<std::string>>(src);

			write(dest, buff);
		}

		if (s.st_mode & S_IFDIR) {
			mkdir(dest.c_str(), S_IRWXU);

			std::vector<std::map<std::string, std::string>> tree = ls(src);
			for (int i = 0; i < tree.size(); i++) {
				if (tree[i]["name"] == path::prev) {
					tree.erase(tree.begin() + i);
				}
			}

			for (int i = 0; i < tree.size(); i++) {
				if (s.st_mode & S_IFREG) {
					std::vector<std::string> buff = rd<std::vector<std::string>>(path::build({
						src,
						tree[i]["name"]
					}));

					write(path::build({
						dest,
						tree[i]["name"]
					}), buff);
				}

				if (s.st_mode & S_IFDIR) {
					cp(path::build({
						src,
						tree[i]["name"]
					}), path::build({
						dest,
						tree[i]["name"]
					}));
				}
			}
		}
	} else {
		omni::err(omni::ERR_FS_NO_ENTRY, {
			src
		});
	}
}

std::vector<std::map<std::string, std::string>> util::fs::ls(std::string path) {
	std::string full = path::build({
		path::curr,
		path
	});
	auto dir = opendir(full.c_str());

	if (!dir) {
		omni::err(omni::ERR_FS_OPEN_DIR, {
			path
		});
	}

	std::vector<std::map<std::string, std::string>> tree;
	while (auto entry = readdir(dir)) {
		if (std::string(entry->d_name) != path::curr && std::string(entry->d_name) != path::prev) {
			std::string type;
			if (entry->d_type == DT_REG) {
				type = "file";
			}

			if (entry->d_type == DT_DIR) {
				type = "dir";
			}

			tree.push_back({
				{
					"name",
					std::string(entry->d_name)
				}, {
					"type",
					type
				}
			});
		}
	}

	closedir(dir);

	return tree;
}


std::string util::fs::perm(std::string fName) {
	std::string _;

	struct stat f;
	if (stat(fName.c_str(), &f) < 0) {
		return _;
	}

	_.push_back(f.st_mode & S_IRUSR ? 'r' : '-');
	_.push_back(f.st_mode & S_IWUSR ? 'w' : '-');
	_.push_back(f.st_mode & S_IXUSR ? 'x' : '-');

	return _;
}

void util::fs::setW(std::string path) {
	struct stat s;
	stat(path.c_str(), &s);

	if (s.st_mode & S_IFREG) {
		chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
	}

	if (s.st_mode & S_IFDIR) {
		chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);

		auto dir = opendir(std::string(path).c_str());
		while (auto entry = readdir(dir)) {
			if (std::string(entry->d_name) != util::fs::path::curr && std::string(entry->d_name) != util::fs::path::prev) {
				if (entry->d_type == DT_REG) {
					chmod(std::string(path::build({
						path,
						entry->d_name
					})).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
				}

				if (entry->d_type == DT_DIR) {
					setW(path::build({
						path,
						std::string(entry->d_name)
					}));
				}
			}
		}
	}
}

std::string util::fs::path::name(std::string path) {
	std::vector<std::string> tok = path::tok(path);

	std::string f;
	if (tok.size()) {
		f = tok.back();
	}

	return f;
}

std::string util::fs::path::base(std::string path) {
	std::string f = name(path);

	return str::split(f, '.')[0];
}

std::vector<std::string> util::fs::path::tok(std::string buff) {
	std::vector<std::string> _;

	unsigned int i = 0;
	while (i < buff.size()) {
		if (std::string(1, buff[i]) == sep) {
			_.push_back(std::string(1, buff[i]));

			i++;
		} else {
			if (std::string(1, buff[i]) == home) {
				_.push_back(home);

				i++;
			} else {
				std::string entry;
				while (
					std::string(1, buff[i]) != sep &&
					i < buff.size()
				) {
					entry.push_back(buff[i]);

					i++;
				}

				_.push_back(entry);
			}
		}
	}

	return _;
}

std::vector<std::string> util::fs::path::entry(std::string buff) {
	std::vector<std::string> _;

	std::vector<std::string> tok = util::fs::path::tok(buff);

	for (int i = 0; i < tok.size(); i++) {
		if (tok[i] != sep) {
			if (tok[i] == home) {
				_.push_back(homeExpand);
			} else if (tok[i] == curr) {
			} else if (tok[i] == prev) {
				if (_.size()) {
					_.pop_back();
				} else {
					omni::err(omni::ERR_FS_ROOT_DIR);
				}
			} else {
				_.push_back(tok[i]);
			}
		}
	}

	return _;
}

std::string util::fs::path::build(std::vector<std::string> entry) {
	entry.insert(entry.begin(), path::curr);

	std::string serial;
	for (int i = 0; i < entry.size(); i++) {
		std::string val = entry[i];

		if (i < entry.size() - 1) {
			val += sep;
		}

		serial += val;
	}

	return serial;
}

std::string util::fs::path::append(std::string lhs, std::string rhs) {
	std::vector<std::string> entryLhs = entry(lhs);
	std::vector<std::string> entryRhs = entry(rhs);

	std::vector<std::string> entry;

	for (int i = 0; i < entryLhs.size(); i++) {
		entry.push_back(entryLhs[i]);
	}

	for (int i = 0; i < entryRhs.size(); i++) {
		entry.push_back(entryRhs[i]);
	}

	return build(entry);
}

std::string util::fs::path::prune(std::string path, std::string name) {
	std::vector<std::string> entryOld = entry(path);

	std::vector<std::string> entryNew;
	for (int i = 0; i < entryOld.size(); i++) {
		if (entryOld[i] == name) {
			break;
		}

		entryNew.push_back(entryOld[i]);
	}

	return build(entryNew);
}

std::vector<GLfloat> util::mesh::plane(glm::vec2 bound) {
	std::vector<GLfloat> vtc;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc.push_back(x * bound[X]);
			vtc.push_back(y * bound[Y]);
		}
	}

	return vtc;
}

std::vector<GLfloat> util::mesh::rd::vtc(std::string fName) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = fs::rd<std::vector<std::string>>("res/obj/" + fName + ".obj");

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = str::split(buff[l], ' ');

		if (tok[0] == "v") {
			omni::assert(tok.size() == 1 + 3, std::string("Couldn't load object `" ) + fName + std::string("`; inappropriate size of vertex position at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + 3; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::rd::st(std::string fName) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = fs::rd<std::vector<std::string>>("res/obj/" + fName + ".obj");

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = str::split(buff[l], ' ');

		if (tok[0] == "vt") {
			omni::assert(tok.size() == 1 + 2, std::string("Couldn't load object `" ) + fName + std::string("`; inappropriate size of texture coordinate at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + 2; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

std::vector<GLfloat> util::mesh::rd::norm(std::string fName) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = fs::rd<std::vector<std::string>>("res/obj/" + fName + ".obj");

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = str::split(buff[l], ' ');

		if (tok[0] == "vn") {
			omni::assert(tok.size() == 1 + 3, std::string("Couldn't load object `" ) + fName + std::string("`; inappropriate size of normal at [" ) + std::to_string(l) + std::string("]"));

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
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 2; b++) {
			bound[a][b] = vtc[a];
		}
	}

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

std::vector<GLfloat> util::mesh::rect::pos(glm::vec2 bound, unsigned int up, bool norm) {
	std::vector<GLfloat> vtc;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc.push_back((x ? 1 : (norm ? -1 : 0)) * bound[X]);

			if (up == Y) {
				vtc.push_back((y ? 1 : (norm ? -1 : 0)) * bound[Y]);
				vtc.push_back(0.0);
			}

			if (up == Z) {
				vtc.push_back(0.0);
				vtc.push_back((y ? 1 : (norm ? -1 : 0)) * bound[Y]);
			}
		}
	}

	return vtc;
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
	glm::vec3 u = vtc[1] - vtc[0];
	glm::vec3 v = vtc[2] - vtc[0];

	// calculate cross product
	glm::vec3 orth = glm::cross(u, v);

	// normalize
	glm::vec3 norm = glm::normalize(orth);

	for (int a = 0; a < 3; a++) {
		_.push_back(norm[a]);
	}

	return _;
}

std::vector<GLushort> util::mesh::rd::idc(std::string fName, unsigned int attr) {
	std::vector<GLushort> _;

	std::vector<std::string> buff = fs::rd<std::vector<std::string>>("res/obj/" + fName + ".obj");

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = str::split(buff[l], ' ');

		if (tok[0] == "f") {
			omni::assert(tok.size() == 1 + 3, std::string("Couldn't load object `" ) + fName + std::string("`; inappropriate number of indices for triangle primitive at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + 3; i++) {
				std::vector<std::string> type = str::split(tok[i], '/');

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
		// P
		glm::vec3 rngP[2];
		for (int b = 0; b < 2; b++) {
			rngP[b] = glm::vec3(0.0);
		}

		for (int b = 0; b < 2; b++) {
			rngP[b][a] = p->_aabb[a][b];

			rngP[b] = glm::vec3(modelP * glm::vec4(rngP[b], 1.0));
		}

		// Q
		glm::vec3 rngQ[2];
		for (int b = 0; b < 2; b++) {
			rngQ[b] = glm::vec3(0.0);
		}

		for (int b = 0; b < 2; b++) {
			rngQ[b][a] = q->_aabb[a][b];

			rngQ[b] = glm::vec3(modelQ * glm::vec4(rngQ[b], 1.0));
		}

		if (!(rngP[MIN][a] < rngQ[MAX][a] && rngP[MAX][a] > rngQ[MIN][a])) {
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
	glm::vec3 btm = matr::apply(glm::vec3(0.0, obj->_aabb[Y][MIN], 0.0), model);

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
	char* _ = (char*) malloc((no + 1));
	for (int i = 0; i < no; i++) {
		_[i] = buff[i];
	}
	_[no] = '\0';

	return _;
}

char util::json::byte(nlohmann::json deser) {
	omni::assert(ascii(deser), std::string("Data not ASCII applicable"));

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

	_._ptr = (char*) malloc(_._x);

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
	_._ptr = (char*) malloc(_._x);
	int c = 0;
	for (int i = 0; i < _._x; i++) {
		omni::assert(ascii(deser[i]), std::string("Data at index [") + std::to_string(i) + std::string("] not ASCII applicable"));

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
	_._ptr = (char*) malloc(_._x * _._y);
	int c = 0;
	for (int j = 0; j < _._y; j++) {
		for (int i = 0; i < _._x; i++) {
			omni::assert(ascii(deser[j][i]), std::string("Data at index [") + std::to_string(j) + "][" + std::to_string(i) + std::string("] not ASCII applicable"));

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
	_._ptr = (char*) malloc(_._x * _._y * _._x);
	int c = 0;
	for (int k = 0; k < _._y; k++) {
		for (int j = 0; j < _._y; j++) {
			for (int i = 0; i < _._x; i++) {
				omni::assert(ascii(deser[k][j][i]), std::string("Data at index [") + std::to_string(j) + "][" + std::to_string(i) + std::string("] not ASCII applicable"));

				((char*) _._ptr)[c] = (char) ((int) deser[k][j][i]);

				c++;
			}
		}
	}

	return _;
}

Var* util::json::var(nlohmann::json key, nlohmann::json val) {
	Var* _;

	char* name = id(key);

	glm::vec3 loc = glm::vec3(0.0);
	if (val.contains("loc")) {
		loc = vec(val["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (val.contains("rot")) {
		rot = vec(val["rot"]);
	}

	switch (val["block"].type()) {
		// scalar
		case nlohmann::json::value_t::number_unsigned: {
			char init = byte(val["block"]);

			Idx* idx = idxMk(0, &init, 1, key, loc, rot);

			omni::assert(!(phys::collGround(idx->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			_ = varMk(name, idx);

			break;
		}

		// array
		case nlohmann::json::value_t::array: {
			switch (val["block"][0].type()) {
				// 1D
				case nlohmann::json::value_t::number_unsigned: {
					CBuff init = array::array(val["block"]);

					Array* val = arrayMk((char*) init._ptr, init._x, key, X, loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

					omni::assert(!(phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

					_ = varMk(name, val);

					break;
				}

				// matrix
				case nlohmann::json::value_t::array: {
					switch (val["block"][0][0].type()) {
						// 2D
						case nlohmann::json::value_t::number_unsigned: {
							CBuff init = array::matr(val["block"]);

							Array* val = arrayMk((char*) init._ptr, init._x, init._y, key, loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

							omni::assert(!(phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

							_ = varMk(name, val);

							break;
						}

						// 3D
						case nlohmann::json::value_t::array: {
							CBuff init = array::tens(val["block"]);

							Array* val = arrayMk((char*) init._ptr, init._x, init._y, key, loc, rot);

							omni::assert(!(phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

							_ = varMk(name, val);

							break;
						}
					}
				}
			}

			break;
		}

		// string
		case nlohmann::json::value_t::string: {
			CBuff init = str(val["block"]);

			Array* val = arrayMk((char*) init._ptr, init._x, key, X, loc + glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))), rot);

			omni::assert(!(phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			_ = varMk(name, val);

			break;
		}

		// dictionary
		case nlohmann::json::value_t::object: {
			nlohmann::json block = val["block"];
			Dict* val = dictMk(block);

			omni::assert(!(phys::collGround(val->_parent)), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			_ = varMk(name, val);

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
		char* name = id(pair.key());

		glm::vec3 loc = glm::vec3(0.0);
		if (pair.value().contains("loc")) {
			loc = vec(pair.value()["loc"]);
		}

		glm::vec3 rot = glm::vec3(0.0);
		if (pair.value().contains("rot")) {
			rot = vec(pair.value()["rot"]);
		}

		switch (pair.value()["block"].type()) {
			// scalar
			case nlohmann::json::value_t::number_unsigned: {
				Var* item = var(pair.key(), pair.value());

				((Var**) _._ptr)[i] = item;
				((unsigned int*) _._type)[i] = omni::SCALAR;

				break;
			}

			// string
			case nlohmann::json::value_t::string: {
				Var* item = var(pair.key(), pair.value());

				((Var**) _._ptr)[i] = item;
				((unsigned int*) _._type)[i] = omni::ARRAY;

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				omni::assert(array::euclid(pair.value()["block"], 0), std::string("Depth of `") + pair.key() + std::string("` exceeds 3 dimensions"));

				switch (pair.value()["block"][0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						Var* item = var(pair.key(), pair.value());

						((Var**) _._ptr)[i] = item;
						((unsigned int*) _._type)[i] = omni::ARRAY;

						break;
					}

					// matrix
					case nlohmann::json::value_t::array: {
						switch (pair.value()["block"][0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								Var* item = var(pair.key(), pair.value());

								((Var**) _._ptr)[i] = item;
								((unsigned int*) _._type)[i] = omni::ARRAY;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								Var* item = json::var(pair.key(), pair.value());

								((Var**) _._ptr)[i] = item;
								((unsigned int*) _._type)[i] = omni::ARRAY;

								break;
							}
						}

						break;
					}
				}

				break;
			}

			// dictionary
			case nlohmann::json::value_t::object: {
				Var* item = var(pair.key(), pair.value());

				((Var**) _._ptr)[i] = item;
				((unsigned int*) _._type)[i] = omni::DICT;

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
	std::vector<bool> pass = ls<bool>(deser["pass"]);

	unsigned int no = pass.size();
	bool* array = (bool*) malloc(no * sizeof (bool));
	for (int i = 0; i < no; i++) {
		array[i] = pass[i];
	}

	glm::vec3 loc = glm::vec3(0.0);
	if (deser.contains("loc")) {
		loc = vec(deser["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (deser.contains("rot")) {
		rot = vec(deser["rot"]);
	}

	StreetSign* _ = streetSignMk({
		array,
		no
	}, loc, rot);

	omni::assert(!phys::collGround(_->_parent), "Street sign clipping into ground plane");

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
		loc = vec(deser["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (deser.contains("rot")) {
		rot = vec(deser["rot"]);
	}

	Obj* _ = objMk(deser["name"], "obj", "dir", false, loc, rot);

	omni::assert(!phys::collGround(_), std::string("Prop `") + std::string(deser["name"]) + std::string("` clipping into ground plane"));

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

	omni::assert(!phys::collGround(_->_parent), "Limit clipping into ground plane");

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
		loc = vec(deser["loc"]);
	}

	Cone* _ = coneMk(init, loc);

	omni::assert(!phys::collGround(_->_parent), "Cone clipping into ground plane");

	return _;
}

bool util::cfg::parse::var(std::string buff) {
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

int util::cfg::parse::whole(std::string buff) {
	for (int i = 0; i < buff.size(); i++) {
		omni::assert(isdigit(buff[i]) || buff[i] == '-', "Value `" + buff + "` invalid, whole number required");
	}

	return std::stoi(buff);
}

float util::cfg::parse::prec(std::string buff) {
	bool valid = true;
	unsigned int dot = 0;
	for (int i = 0; i < buff.size(); i++) {
		if (buff[i] == '.') {
			dot++;
		}

		if (
			!isdigit(buff[i]) &&
			buff[i] != '.'
		) {
			valid = false;

			break;
		}

		if (dot > 1) {
			valid = false;

			break;
		}
	}

	omni::assert(valid, "Value `" + buff + "` invalid, number with dot precision required");

	return std::stof(buff);
}

bool util::cfg::parse::boolean(std::string buff) {
	omni::assert(buff.size() == 1, "Value `" + buff + "` invalid, boolean required");

	omni::assert(buff[0] == 'n' || buff[0] == 'y', "Value `" + buff + "` invalid, boolean required");

	bool _;
	switch (buff[0]) {
		case 'n':
			_ = false;

			break;

		case 'y':
			_ = true;

			break;
	}

	return _;
}

std::string util::cfg::parse::str(std::string buff) {
	int i = 0;

	// opening quote
	i++;

	std::string _;
	while (buff[i] != '\'') {
		_.push_back(buff[i]);

		i++;
	}

	// closing quote
	i++;

	return _;
}

std::map<std::string, std::string> util::cfg::lex(std::string name) {
	std::map<std::string, std::string> _;

	std::vector<std::string> buff = fs::rd<std::vector<std::string>>(name);
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
				tok.push_back(line[i]);
				i++;

				while (
					line[i] != '\'' &&
					i < line.size()
				) {
					tok.push_back(line[i]);

					i++;
				}

				// closing quote
				tok.push_back(line[i]);
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
				omni::err(omni::ERR_TOK_CNT);
			}

			if (ast[1] != "=") {
				omni::err(omni::ERR_TOK, {
					ast[1]
				});
			}

			if (!cfg::parse::var(ast[0])) {
				omni::err(omni::ERR_CFG_KEY, {
					ast[0]
				});

				break;
			}

			_[ast[0]] = ast[2];
		}
	}

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

int util::math::delta(int lhs, int rhs) {
	return rhs - lhs;
}

int util::math::norm(int lhs, int rhs) {
	int _ = 0;

	int diff = delta(lhs, rhs);
	if (diff > 0) {
		_ = 1;
	}
	if (diff < 0) {
		_ = -1;
	}

	return _;
}

unsigned int util::math::clamp(unsigned int i, int sign, unsigned int roof) {
	unsigned int _ = i;

	if (sign == 1) {
		if (!(_ < roof)) {
			_ = roof;
		}
	}

	if (sign == -1) {
		if (!(_ > roof)) {
			_ = roof;
		}
	}

	return _;
}

unsigned int util::math::idx::arr(Coord st, Coord bound) {
	return (st._y * bound._x) + st._x;
}

unsigned int util::math::idx::determ(Coord st, std::vector<std::string> buff) {
	unsigned int _ = 0;
	for (int i = 0; i < st._y; i++) {
		_ += buff[i].size();
	}
	_ += st._x;

	return _;
}

unsigned int util::math::idx::expand(std::string line, unsigned int idx) {
	unsigned int _ = 0;

	unsigned int c = 0;
	while (c < idx) {
		if (line[c] == '\t') {
			_ += state::tabWd;
		} else {
			_++;
		}

		c++;
	}

	return _;
}

Coord util::math::coord::arr(unsigned int idx, Coord bound) {
	return {
		idx % bound._x,
		idx / bound._x
	};
}

Coord util::math::coord::determ(unsigned int idx, std::vector<std::string> buff) {
	Coord _ = {
		0,
		0
	};

	int i = 0;
	while (i < idx) {
		if (_._x < buff[_._y].size() - 1) {
			_._x++;
		} else {
			_._y++;
			_._x = 0;
		}

		i++;
	}

	return _;
}

std::vector<std::string> util::log(unsigned int loc, unsigned int ptrEditorX) {
	std::vector<std::string> buff;

	std::string head = "Level complete";
	buff.push_back(head);

	std::string lb;
	for (int i = ptrEditorX; i < state::lineWd - 1; i++) {
		lb.push_back('=');
	}
	buff.push_back(lb);

	std::map<std::string, std::string> attr = {
		{
			"LOC",
			std::to_string(loc)
		}, {
			"CALLS",
			std::to_string(c)
		}
	};

	for (std::map<std::string, std::string>::iterator it = attr.begin(); it != attr.end(); ++it) {
		std::string key = it->first;
		key += ":";

		std::string val = it->second;

		std::string pair = str::pad(key, (state::lineWd - ptrEditorX - 1) - val.size());
		pair += val;

		buff.push_back(pair);
	}

	for (int i = buff.size(); i < state::lineCnt - 2 - 1 - 2 - 1; i++) {
		buff.push_back("");
	}

	buff.push_back("");
	buff.push_back(now(state::format));

	buff.push_back("");
	buff.push_back("Enter `next` to proceed");

	return buff;
}

std::string util::now(std::string format) {
	time_t epoch;
	time(&epoch);

	struct tm* info = localtime(&epoch);

	char out[state::lineWd];
	strftime(out, sizeof out / sizeof *out, format.c_str(), info);

	return std::string(out);
}

GLuint util::tex::spray(std::string tex) {
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
		omni::err(omni::ERR_FRAME_BUFF);
	}

	// create
	std::vector<GLfloat> vtc = mesh::rd::vtc(tex);

	std::vector<GLushort> idc = mesh::rd::idc(tex, Obj::POS);

	Mesh* mesh = meshMk(&vtc[0], &idc[0], idc.size());

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

	glUniform2i(uniRes, disp->_res[X], disp->_res[Y]);

	glUniform1ui(uniActive, true);

	prog.unUse();

	// draw
	disp->clear();

	glBindVertexArray(mesh->_id[Mesh::VAO]);
	prog.use();

	glDrawElements(GL_TRIANGLES, idc.size(), GL_UNSIGNED_SHORT, (GLvoid*) 0);

	prog.unUse();
	glBindVertexArray(0);

	meshDel(mesh);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return cbo;
}
