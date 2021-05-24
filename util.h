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
#include "street_light.h"

namespace util {
	namespace fs {
		template <typename T>
		T rd(std::string name);

		template <>
		std::string rd(std::string name);

		template <>
		std::vector<std::string> rd(std::string name);

		void write(std::string name, std::vector<std::string> buff);

		std::vector<std::map<std::string, std::string>> ls(std::string name);

		std::string name(std::string buff);

		std::string base(std::string buff);

		bool w(std::string name);
	}

	namespace mesh {
		std::vector<GLfloat> plane(glm::vec2 sz);

		std::vector<GLushort> strip(std::vector<GLushort> idc);

		void bound(GLfloat rng[3][2], GLfloat* vtc, unsigned int noVtc);

		namespace rect {
			std::vector<GLfloat> pos(glm::vec2 sz, unsigned int up, bool norm);
			std::vector<GLushort> idc();
		}

		namespace gen {
			std::vector<GLfloat> normTri(std::vector<glm::vec3> vtc);
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
		bool aabb(Obj* p, Obj* q);

		bool aabbGround(Obj* obj);
	}

	namespace str {
		std::vector<std::string> split(std::string buff, char delim);
		std::string join(std::vector<std::string> buff);

		std::string pad(std::string buff, unsigned int roof);
	}

	namespace json {
		char* id(nlohmann::json deser);

		char byte(nlohmann::json deser);

		CBuff arr(nlohmann::json deser);
		CBuff matr2(nlohmann::json deser);
		CBuff matr3(nlohmann::json deser);

		Var** scope(nlohmann::json deser, unsigned int*& type);

		glm::vec3 vec(nlohmann::json deser);

		StreetLight* streetLight(nlohmann::json deser);

		std::vector<Obj*> path(nlohmann::json deser, nlohmann::json node);

		Obj* prop(nlohmann::json deser);

		namespace bound {
			Lim* lim(nlohmann::json key, nlohmann::json val);
			Cone* area(nlohmann::json deser);
		}
	}

	namespace cfg {
		std::string key(std::string buff);

		bool var(std::string buff);

		bool no(std::string buff);

		template <typename T>
		std::map<std::string, T> parse(std::string name);
	}

	namespace tex {
		GLuint spray(char c);
	}

	std::vector<std::string> log(unsigned int loc, unsigned int maxFs);

	std::string now(std::string format);
}
