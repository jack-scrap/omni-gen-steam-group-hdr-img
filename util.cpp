#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <sstream>
#include <dirent.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sys/stat.h>
#include <iomanip>

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
	in.open(path::build({
		fName
	}));

	std::string cont;

	if (exist(fName)) {
		for (std::string l; std::getline(in, l);) {
			cont += l + '\n';
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
	struct stat f;
	if (stat(fName.c_str(), &f) < 0) {
		return "";
	}

	std::string buff;
	buff.push_back(f.st_mode & S_IRUSR ? 'r' : '-');
	buff.push_back(f.st_mode & S_IWUSR ? 'w' : '-');
	buff.push_back(f.st_mode & S_IXUSR ? 'x' : '-');

	return buff;
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

const std::string util::fs::path::sep = "/";

const std::string util::fs::path::home = "~";
const std::string util::fs::path::homeExpand = "player";

const std::string util::fs::path::curr = ".";
const std::string util::fs::path::prev = "..";

std::vector<std::string> util::fs::path::tok(std::string buff) {
	std::vector<std::string> cont;

	int i = 0;
	while (i < buff.size()) {
		if (std::string(1, buff[i]) == sep) {
			cont.push_back(std::string(1, buff[i]));

			i++;
		} else {
			if (std::string(1, buff[i]) == home) {
				cont.push_back(home);

				i++;
			} else {
				std::string entry;
				while (std::string(1, buff[i]) != sep && i < buff.size()) {
					entry.push_back(buff[i]);

					i++;
				}

				cont.push_back(entry);
			}
		}
	}

	return cont;
}

std::vector<std::string> util::fs::path::entry(std::string buff) {
	std::vector<std::string> cont;

	std::vector<std::string> tok = util::fs::path::tok(buff);
	for (int i = 0; i < tok.size(); i++) {
		if (tok[i] != sep) {
			if (tok[i] == home) {
				cont.push_back(homeExpand);
			} else if (tok[i] == prev) {
				if (cont.size()) {
					cont.pop_back();
				} else {
					omni::err(omni::ERR_FS_ROOT_DIR);
				}
			} else {
				cont.push_back(tok[i]);
			}
		}
	}

	return cont;
}

std::string util::fs::path::build(std::vector<std::string> entry) {
	entry = prune(entry, curr);

	entry.insert(entry.begin(), curr);

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
	std::vector<std::string> deser;

	std::vector<std::string> entryLhs = entry(lhs);
	std::vector<std::string> entryRhs = entry(rhs);

	deser.insert(deser.end(), entryLhs.begin(), entryLhs.end());
	deser.insert(deser.end(), entryRhs.begin(), entryRhs.end());

	return build(deser);
}

std::vector<std::string> util::fs::path::prune(std::vector<std::string> path, std::string name) {
	std::vector<std::string> _;
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == name) {
			continue;
		}

		_.push_back(path[i]);
	}

	return _;
}

void util::mesh::plane(GLfloat vtc[2 * 2 * 2], glm::vec2 bound) {
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = x * bound[X];
			vtc[i + 1] = y * bound[Y];

			i++;
		}
	}
}

std::vector<GLfloat> util::mesh::rd::attr(std::string fName, unsigned int attr) {
	const std::string id[3] = {
		"v",
		"vt",
		"vn"
	};
	const unsigned int sz[3] = {
		3,
		2,
		3
	};

	std::vector<std::string> buff = fs::rd<std::vector<std::string>>(fs::path::build({
		"res",
		"obj",
		fName + ".obj"
	}));

	std::vector<GLfloat> cont;
	for (int l = 0; l < buff.size(); l++) {
		if (!buff[l].size()) {
			continue;
		}

		std::vector<std::string> tok = str::split(buff[l], ' ');

		if (tok[0] == id[attr]) {
			omni::assert(tok.size() == 1 + sz[attr], std::string("Couldn't load object `" ) + fName + std::string("`; inappropriate size of vertex position at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + sz[attr]; i++) {
				std::stringstream out;
				out << std::fixed << std::setprecision(4) << std::stof(tok[i]);

				cont.push_back(std::stof(out.str()));
			}
		}
	}

	return cont;
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
				vtc.push_back(0.0);
				vtc.push_back((y ? 1 : (norm ? -1 : 0)) * bound[Y]);
			}

			if (up == Z) {
				vtc.push_back((y ? 1 : (norm ? -1 : 0)) * bound[Y]);
				vtc.push_back(0.0);
			}
		}
	}

	return vtc;
}

std::vector<GLushort> util::mesh::rect::idc() {
	std::vector<GLushort> cont;
	for (int t = 0; t < 2; t++) {
		for (int a = 0; a < 3; a++) {
			GLushort offset = (t * 3);

			cont.push_back(offset + (a * (t ? -1 : 1)));
		}
	}

	return cont;
}

glm::vec3 util::mesh::gen::norm(std::vector<glm::vec3> vtc) {
	// get edges
	glm::vec3 u = vtc[1] - vtc[0];
	glm::vec3 v = vtc[2] - vtc[0];

	// calculate cross product
	glm::vec3 orth = glm::cross(u, v);

	// normalize
	glm::vec3 norm = glm::normalize(orth);

	return norm;
}

std::vector<GLushort> util::mesh::rd::idc(std::string fName, unsigned int attr) {
	std::vector<std::string> buff = fs::rd<std::vector<std::string>>(fs::path::build({
		"res",
		"obj",
		fName + ".obj"
	}));

	std::vector<GLushort> cont;
	for (int l = 0; l < buff.size(); l++) {
		if (!buff[l].size()) {
			continue;
		}

		std::vector<std::string> tok = str::split(buff[l], ' ');

		if (tok[0] == "f") {
			omni::assert(tok.size() == 1 + 3, std::string("Couldn't load object `" ) + fName + std::string("`; inappropriate number of indices for triangle primitive at [" ) + std::to_string(l) + std::string("]"));

			for (int i = 1; i < 1 + 3; i++) {
				std::vector<std::string> type = str::split(tok[i], '/');

				cont.push_back(std::stoi(type[attr]) - 1);
			}
		}
	}

	return cont;
}

glm::mat4 util::matr::rot(glm::mat4 model, glm::vec3 rot) {
	glm::quat quat = glm::quat(rot);

	return model * glm::mat4_cast(quat);
}

glm::vec3 util::matr::apply(glm::vec3 vtx, glm::mat4 model) {
	return glm::vec3(model * glm::vec4(glm::vec3(0.0), 1.0));
}

bool util::phys::aabb(Obj* p, Obj* q, glm::mat4 modelP, glm::mat4 modelQ) {
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

		// collision
		if (!(rngP[MIN][a] < rngQ[MAX][a] && rngP[MAX][a] > rngQ[MIN][a])) {
			_ = false;

			break;
		}
	}

	return _;
}

bool util::phys::aabb(glm::vec3 vtx, Obj* bound, glm::mat4 modelVtx, glm::mat4 modelBound) {
	bool _ = true;

	glm::vec3 vtxTrans = glm::vec3(modelVtx * glm::vec4(vtx, 1.0));

	for (int a = 0; a < 3; a++) {
		glm::vec3 rng[2];
		for (int i = 0; i < 2; i++) {
			rng[i] = glm::vec3(0.0);
		}

		rng[MIN][a] = bound->_aabb[a][MIN];
		rng[MIN] = glm::vec3(modelBound * glm::vec4(rng[MIN], 1.0));

		rng[MAX][a] = bound->_aabb[a][MAX];
		rng[MAX] = glm::vec3(modelBound * glm::vec4(rng[MAX], 1.0));

		if (!(vtxTrans[a] > rng[MIN][a] && vtxTrans[a] < rng[MAX][a])) {
			_ = false;

			break;
		}
	}

	return _;
}

bool util::phys::aabbGround(Obj* obj, glm::mat4 model) {
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

bool util::str::ascii(char c) {
	return c >= 0 && c <= 127;
}

char* util::json::id(nlohmann::json deser) {
	std::string buff = deser.get<std::string>();

	unsigned int no = buff.size();
	char* _ = (char*) malloc(no + 1);
	for (int i = 0; i < no; i++) {
		_[i] = buff[i];
	}
	_[no] = '\0';

	return _;
}

char util::json::byte(nlohmann::json deser) {
	omni::assert(str::ascii((char) ((int) deser)), std::string("Data not ASCII applicable"));

	return (char) ((int) deser);
}

CBuff util::json::array::dim(nlohmann::json deser, CBuff buff, int i) {
	switch (i) {
		case 0:
			buff.z = deser.size();

			break;

		case 1:
			buff.y = deser.size();

			break;

		case 2:
			buff.x = deser.size();

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
	CBuff cBuff;

	cBuff.y = 1;
	cBuff.z = 1;

	std::string buff = deser;

	cBuff.x = buff.size();

	cBuff.ptr = (char*) malloc(cBuff.x);

	for (int i = 0; i < buff.size(); i++) {
		((char*) cBuff.ptr)[i] = buff[i];
	}

	return cBuff;
}

StreetSign* util::json::streetSign(nlohmann::json deser) {
	std::vector<bool> status = ls<bool>(deser["status"]);

	unsigned int no = status.size();
	bool* array = (bool*) malloc(no * sizeof (bool));
	for (int i = 0; i < no; i++) {
		array[i] = status[i];
	}

	glm::vec3 loc = glm::vec3(0.0);
	if (deser.contains("loc")) {
		loc = vec(deser["loc"]);
	}

	glm::vec3 rot = glm::vec3(0.0);
	if (deser.contains("rot")) {
		rot = vec(deser["rot"]);
	}

	StreetSign* inst = streetSignMk({
		array,
		no
	}, loc, glm::radians(rot));

	omni::assert(!phys::aabbGround(inst->_parent), "Street sign clipping into ground plane");

	return inst;
}

CBuff util::json::array::lin(nlohmann::json deser) {
	CBuff cBuff;

	cBuff.x = deser.size();
	cBuff.y = 1;
	cBuff.z = 1;
	cBuff.ptr = (char*) malloc(cBuff.x);
	for (int i = 0; i < cBuff.x; i++) {
		omni::assert(str::ascii((char) ((int) deser[i])), std::string("Data at index [") + std::to_string(i) + std::string("] not ASCII applicable"));

		((char*) cBuff.ptr)[i] = byte(deser[i]);
	}

	return cBuff;
}

CBuff util::json::array::matrix(nlohmann::json deser) {
	CBuff cBuff;

	cBuff.x = deser[0].size();
	cBuff.y = deser.size();
	cBuff.z = 1;
	cBuff.ptr = (char*) malloc(cBuff.x * cBuff.y);
	for (int j = 0; j < cBuff.y; j++) {
		for (int i = 0; i < cBuff.x; i++) {
			omni::assert(str::ascii((char) ((int) deser[j][i])), std::string("Data at index [") + std::to_string(j) + "][" + std::to_string(i) + std::string("] not ASCII applicable"));

			int c = util::math::idx::array({
				i,
				j
			}, {
				cBuff.x,
				cBuff.y
			});

			((char*) cBuff.ptr)[c] = byte(deser[i]);
		}
	}

	return cBuff;
}

Var* util::json::var(nlohmann::json key, nlohmann::json val, glm::vec3 loc, glm::vec3 rot) {
	char* name = id(key);

	if (val.contains("loc")) {
		loc = vec(val["loc"]);
	}

	if (val.contains("rot")) {
		rot = vec(val["rot"]);
	}

	Var* inst;
	switch (val["block"].type()) {
		// array
		case nlohmann::json::value_t::array: {
			if (val["block"].empty()) {
				CBuff data = array::lin(val["block"]);

				Idx* idx;
				if (data.x) {
					idx = idxMk(0, (char*) data.ptr, data.x, key, loc, glm::radians(rot));
				} else {
					idx = idxMk(0, key, loc, glm::radians(rot));
				}

				omni::assert(!phys::aabbGround(idx->_parent), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

				glm::vec3 offset = util::matr::apply(glm::vec3(0.0), idx->_parent->_acc);

				inst = varMk(name, idx, omni::IDX, offset);
			} else {
				switch (val["block"][0].type()) {
					// stack
					case nlohmann::json::value_t::number_unsigned: {
						CBuff data = array::lin(val["block"]);

						Idx* idx;
						if (data.x) {
							idx = idxMk(0, (char*) data.ptr, data.x, key, loc, glm::radians(rot));
						} else {
							idx = idxMk(0, key, loc, glm::radians(rot));
						}

						omni::assert(!phys::aabbGround(idx->_parent), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

						glm::vec3 offset = util::matr::apply(glm::vec3(0.0), idx->_parent->_acc);

						inst = varMk(name, idx, omni::IDX, offset);

						break;
				 	}
				}
			}

			break;
		}

		// string
		case nlohmann::json::value_t::string: {
			CBuff data = str(val["block"]);

			Array* array = arrayMk((char*) data.ptr, data.x, key, X, loc + glm::vec3(0.0, 0.0, -(layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2)), glm::radians(rot));

			omni::assert(!phys::aabbGround(array->_parent), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			glm::vec3 offset = util::matr::apply(glm::vec3(0.0), array->_parent->_acc);

			inst = varMk(name, array, omni::ARRAY, offset);

			break;
		}

		// dictionary
		case nlohmann::json::value_t::object: {
			nlohmann::json block = val["block"];
			Dict* dict = dictMk(block, key);

			omni::assert(!phys::aabbGround(dict->_parent), std::string("Data `") + std::string(key) + std::string("` clipping into ground plane"));

			glm::vec3 offset = util::matr::apply(glm::vec3(0.0), dict->_parent->_acc);

			inst = varMk(name, dict, omni::DICT, offset);

			break;
		}
	}

	return inst;
}

glm::vec3 util::json::vec(nlohmann::json deser) {
	glm::vec3 _;
	for (int a = 0; a < 3; a++) {
		_[a] = deser[a];
	}

	return _;
}

std::vector<Obj*> util::json::path(nlohmann::json path, nlohmann::json node, nlohmann::json status) {
	std::vector<Obj*> root;

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
		name = "alert_road";
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

				root.push_back(lineMk((GLfloat*) vtc, idc, 2, "main", "lane", name, active));

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

					root.push_back(lineMk((GLfloat*) vtc, idc, 2, "obj", "thick", name, active));
				}

				break;
			}
		}
	}

	return root;
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

	Obj* inst = objMk(deser["name"], "obj", "dir", false, loc, glm::radians(rot));

	omni::assert(!phys::aabbGround(inst), std::string("Prop `") + std::string(deser["name"]) + std::string("` clipping into ground plane"));

	return inst;
}

Mark* util::json::bound::mark(nlohmann::json deser) {
	GLfloat data[2][2];
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 2; i++) {
			data[j][i] = deser["val"][j][i];
		}
	}

	unsigned int status = HALT;
	if (deser["status"] == "pass") {
		status = PASS;
	}

	if (deser["status"] == "alert") {
		status = ALERT;
	}

	Mark* inst = markMk(&data[0], status);

	omni::assert(!phys::aabbGround(inst->_parent), "Marking clipping into ground plane");

	return inst;
}

Lim* util::json::bound::lim(nlohmann::json deser) {
	unsigned int axis;
	if (deser["axis"] == "X") {
		axis = X;
	}

	if (deser["axis"] == "Z") {
		axis = Z;
	}

	unsigned int status = HALT;
	if (deser["status"] == "pass") {
		status = PASS;
	}

	if (deser["status"] == "alert") {
		status = ALERT;
	}

	Lim* inst = limMk(axis, deser["val"], status);

	omni::assert(!phys::aabbGround(inst->_parent), "Limit clipping into ground plane");

	return inst;
}

Cone* util::json::bound::area(nlohmann::json deser) {
	GLfloat data[2][2];
	for (int a = 0; a < 2; a++) {
		for (int b = 0; b < 2; b++) {
			data[a][b] = deser["bound"][a][b];
		}
	}

	glm::vec3 loc = glm::vec3(0.0);
	if (deser.contains("loc")) {
		loc = vec(deser["loc"]);
	}

	Cone* inst = coneMk(data, loc);

	omni::assert(!phys::aabbGround(inst->_parent), "Cone clipping into ground plane");

	return inst;
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

		if (!isdigit(buff[i]) && buff[i] != '.') {
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

				while (line[i] != '\'' && i < line.size()) {
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

				while (!isspace(line[i]) && i < line.size()) {
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

unsigned int util::math::idx::array(Coord st, Coord bound) {
	return (st.y * bound.x) + st.x;
}

unsigned int util::math::idx::determ(Coord st, std::vector<std::string> buff) {
	unsigned int i = 0;
	for (int l = 0; l < st.y; l++) {
		i += buff[l].size();
	}
	i += st.x;

	return i;
}

unsigned int util::math::idx::expand(std::string line, unsigned int idx) {
	unsigned int i = 0;

	unsigned int c = 0;
	while (c < idx) {
		if (line[c] == '\t') {
			i += state::tabWd;
		} else {
			i++;
		}

		c++;
	}

	return i;
}

Coord util::math::coord::array(unsigned int idx, Coord bound) {
	return {
		idx % bound.x,
		idx / bound.x
	};
}

Coord util::math::coord::determ(unsigned int idx, std::vector<std::string> buff) {
	Coord coord = {
		0,
		0
	};

	int i = 0;
	while (i < idx) {
		if (coord.x < buff[coord.y].size() - 1) {
			coord.x++;
		} else {
			coord.y++;
			coord.x = 0;
		}

		i++;
	}

	return coord;
}

std::string util::now(std::string format) {
	time_t epoch;
	time(&epoch);

	struct tm* info = localtime(&epoch);

	char out[state::lineWd];
	strftime(out, sizeof out / sizeof *out, format.c_str(), info);

	return std::string(out);
}

GLuint util::tex::rd(std::string name) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	std::string path = util::fs::path::build({
		"res",
		name + ".bmp"
	});

	if (!util::fs::exist(path)) {
		omni::err(omni::ERR_FS_NO_ENTRY, {
			path
		});

		return 0;
	}

	SDL_Surface* surf = SDL_LoadBMP(path.c_str());

	SDL_Texture* texSdl = SDL_CreateTextureFromSurface(disp->rend, surf);

	int dim[2];
	SDL_QueryTexture(texSdl, nullptr, nullptr, &dim[0], &dim[1]);

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = dim[X];
	rect.h = dim[Y];

	SDL_RenderCopy(disp->rend, texSdl, &rect, &rect);

	if (!surf->pixels) {
		omni::err(omni::ERR_RD_TEX, {
			name + ".bmp"
		});

		return 0;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rect.w, rect.h, 0, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
	std::vector<GLfloat> vtc = mesh::rd::attr(tex, Obj::POS);
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

std::string util::glyphName(char c) {
	std::string name;

	// alpha-numeric
	if (
		(c >= '0' && c <= '9') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z')
	) {
		name = std::string(1, c);
	} else {
		switch (c) {
			case ' ':
				name = "spc";

				break;

			case '!':
				name = "bang";

				break;

			case '"':
				name = "dbl_quote";

				break;

			case '#':
				name = "pound";

				break;

			case '$':
				name = "dollar";

				break;

			case '%':
				name = "percent";

				break;

			case '&':
				name = "ampersand";

				break;

			case '\'':
				name = "single_quote";

				break;

			case '(':
				name = "round_bracket_l";

				break;

			case ')':
				name = "round_bracket_r";

				break;

			case '*':
				name = "asterisk";

				break;

			case '+':
				name = "plus";

				break;

			case ',':
				name = "comma";

				break;

			case '-':
				name = "minus";

				break;

			case '.':
				name = "period";

				break;

			case '/':
				name = "fwd_slash";

				break;

			case ':':
				name = "colon";

				break;

			case ';':
				name = "semi_colon";

				break;

			case '<':
				name = "less_than";

				break;

			case '=':
				name = "equals";

				break;

			case '>':
				name = "greater_than";

				break;

			case '?':
				name = "question";

				break;

			case '@':
				name = "at";

				break;

			case '[':
				name = "sq_bracket_l";

				break;

			case ']':
				name = "sq_bracket_r";

				break;

			case '^':
				name = "carat";

				break;

			case '_':
				name = "underscore";

				break;

			case '`':
				name = "back_tick";

				break;

			case '{':
				name = "curly_bracket_l";

				break;

			case '|':
				name = "pipe";

				break;

			case '}':
				name = "curly_bracket_r";

				break;

			case '~':
				name = "tilde";

				break;
		}
	}

	return name;
}
