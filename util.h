#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "json.hpp"

#include "obj.h"
#include "c_buff.h"
#include "var.h"
#include "lim.h"
#include "cone.h"
#include "street_sign.h"
#include "math.h"

namespace util {
	namespace fs {
		template <typename T>
		T rd(std::string fName);

		template <>
		std::string rd(std::string fName);

		template <>
		std::vector<std::string> rd(std::string fName);

		bool exist(std::string fName);

		void write(std::string fName, std::vector<std::string> buff);

		void del(std::string fName);

		std::vector<std::map<std::string, std::string>> ls(std::string path);

		std::string perm(std::string fName);

		std::string name(std::string path);

		std::string base(std::string path);

		namespace path {
			const std::string sep = "/";

			const std::string home = "~";
			const std::string homeExpand = "asdf";

			const std::string curr = ".";
			const std::string prev = "..";

			std::vector<std::string> tok(std::string buff);

			std::vector<std::string> entry(std::vector<std::string> tok);

			std::string build(std::vector<std::string> entry);

			std::string append(std::string lhs, std::string rhs);

			std::string prune(std::string path, std::string name);
		}
	}

	namespace mesh {
		std::vector<GLfloat> plane(glm::vec2 bound);

		void strip(GLushort idc[2][3]);

		void aabb(GLfloat bound[3][2], GLfloat* vtc, GLushort* idc, unsigned int noPrim);

		namespace rect {
			std::vector<GLfloat> pos(glm::vec2 bound, unsigned int up, bool norm);
			std::vector<GLushort> idc();
		}

		namespace gen {
			std::vector<GLfloat> norm(std::vector<glm::vec3> vtc);
		}

		namespace rd {
			std::vector<GLfloat> vtc(std::string fName);
			std::vector<GLfloat> st(std::string fName);
			std::vector<GLfloat> norm(std::string fName);
			std::vector<GLushort> idc(std::string fName, unsigned int attr);
		}
	}

	namespace matr {
		glm::mat4 rot(glm::mat4 model, glm::vec3 rot);

		glm::vec3 apply(glm::vec3 vtx, glm::mat4 model);
	}

	namespace phys {
		bool coll(Obj* p, Obj* q, glm::mat4 modelP = glm::mat4(1.0), glm::mat4 modelQ = glm::mat4(1.0));
		bool coll(glm::vec3 vtx, Obj* bound, glm::mat4 modelBound = glm::mat4(1.0), glm::mat4 modelVtx = glm::mat4(1.0));

		bool collGround(Obj* obj, glm::mat4 model = glm::mat4(1.0));
	}

	namespace str {
		std::vector<std::string> split(std::string buff, char delim);
		std::string join(std::vector<std::string> buff);

		std::string pad(std::string buff, unsigned int roof);
	}

	namespace json {
		char* id(nlohmann::json deser);

		char byte(nlohmann::json deser);

		bool ascii(nlohmann::json deser);

		Var* var(nlohmann::json key, nlohmann::json val);

		Scope scope(nlohmann::json deser);

		glm::vec3 vec(nlohmann::json deser);

		StreetSign* streetSign(nlohmann::json deser);

		std::vector<Obj*> path(nlohmann::json path, nlohmann::json node, nlohmann::json status);

		Obj* prop(nlohmann::json deser);

		template <typename T>
		std::vector<T> ls(nlohmann::json deser);

		CBuff str(nlohmann::json deser);

		namespace array {
			CBuff dim(nlohmann::json deser, CBuff buff, int i);

			bool euclid(nlohmann::json deser, unsigned int sz);

			CBuff array(nlohmann::json deser);
			CBuff matr(nlohmann::json deser);
			CBuff tens(nlohmann::json deser);
		}

		namespace bound {
			Lim* lim(nlohmann::json val);
			Cone* area(nlohmann::json deser);
		}
	}

	namespace cfg {
		namespace parse {
			bool var(std::string buff);

			bool boolean(std::string buff);

			int whole(std::string buff);

			float prec(std::string buff);

			std::string str(std::string buff);
		}

		std::map<std::string, std::string> lex(std::string name);

		std::string key(std::string buff);
	}

	namespace tex {
		GLuint spray(std::string tex);
	}

	namespace math {
		int delta(int min, int max);

		int norm(int lhs, int rhs);

		unsigned int clamp(unsigned int i, int dir, unsigned int roof);

		namespace idx {
			unsigned int arr(Coord st, Coord bound);
			unsigned int determ(Coord st, std::vector<std::string> buff);

			unsigned int expand(std::string line, unsigned int idx);
		}

		namespace coord {
			Coord arr(unsigned int idx, Coord bound);
			Coord determ(unsigned int idx, std::vector<std::string> buff);
		}
	}

	std::vector<std::string> log(unsigned int loc, unsigned int ptrEditorX);

	std::string now(std::string format);
}
