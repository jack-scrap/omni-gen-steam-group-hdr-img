#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>

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
		T rd(std::string name);

		template <>
		std::string rd(std::string name);

		template <>
		std::vector<std::string> rd(std::string name);

		void write(std::string name, std::vector<std::string> buff);

		void del(std::string name);

		std::vector<std::map<std::string, std::string>> ls(std::string name);

		std::string name(std::string buff);

		std::string base(std::string buff);

		std::string perm(std::string name);
	}

	namespace mesh {
		std::vector<GLfloat> plane(glm::vec2 sz);

		void strip(GLushort idc[2][3]);

		void aabb(GLfloat bound[3][2], GLfloat* vtc, GLushort* idc, unsigned int noPrim);

		namespace rect {
			std::vector<GLfloat> pos(glm::vec2 sz, unsigned int up, bool norm);
			std::vector<GLushort> idc();
		}

		namespace gen {
			std::vector<GLfloat> norm(std::vector<glm::vec3> vtc);
		}

		namespace rd {
			std::vector<GLfloat> vtc(std::string name);
			std::vector<GLfloat> st(std::string name);
			std::vector<GLfloat> norm(std::string name);
			std::vector<GLushort> idc(std::string name, unsigned int attr);
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

			int no(std::string buff);

			bool boolean(std::string buff);

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

		namespace idx {
			unsigned int arr(Coord st, Coord bound);
			unsigned int determ(Coord st, std::vector<std::string> buff);
		}

		namespace coord {
			Coord arr(unsigned int idx, Coord bound);
			Coord determ(unsigned int idx, std::vector<std::string> buff);
		}
	}

	std::vector<std::string> log(unsigned int loc, unsigned int maxFs);

	std::string now(std::string format);
}
