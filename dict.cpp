#include <cstdlib>

#include "dict.h"
#include "str.h"
#include "layout.h"
#include "border.h"
#include "idx.h"
#include "scn.h"
#include "omni.h"
#include "util.h"

Dict* dictMk(nlohmann::json deser, glm::vec3 loc, glm::vec3 rot) {
	Dict* _ = (Dict*) malloc(sizeof (Dict));

	Obj* child[deser.size()];

	glm::vec3 start = glm::vec3(0.0, 0.0, layout::letter[Y] + (layout::margin * 2 * 2)) + glm::vec3(layout::overhead, 0.0, layout::overhead);

	GLfloat szPair = layout::stride[Z] + (layout::margin * 2) + (layout::stroke * 2) + (layout::letter[Y] + (layout::margin * 2)) + (layout::margin * 2 * 2);

	unsigned int i = 0;
	for (const auto& pair : deser.items()) {
		if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
			char* init = (char*) malloc(sizeof (char));
			init[0] = util::json::byte(pair.value());

			Idx* _ = idxMk(i, init, 1, pair.key(), start + glm::vec3(0.0, 0.0, i * szPair));

			child[i] = _->_parent;
		}

		if (pair.value().type() == nlohmann::json::value_t::array) {
			char* init = (char*) malloc(pair.value().size());
			unsigned int sz = pair.value().size();
			for (int i = 0; i < sz; i++) {
				init[i] = (char) ((int) pair.value()[i]);
			}

			Array* _ = arrayMk(init, sz, pair.key(), X, start + glm::vec3(0.0, 0.0, i * szPair));

			child[i] = _->_parent;
		}

		i++;
	}

	unsigned int maxX = 0;
	for (const auto& pair : deser.items()) {
		if (pair.value().size() > maxX) {
			maxX = pair.value().size();
		}
	}

	unsigned int totalZ = 0;
	for (const auto& pair : deser.items()) {
		if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
			totalZ++;
		}

		if (pair.value().type() == nlohmann::json::value_t::array) {
			if (pair.value()[0].type() == nlohmann::json::value_t::array) {
				totalZ += pair.value().size();
			} else {
				totalZ++;
			}
		}
	}

	// scope
	Border* scope = borderMk({
		(layout::margin * 2) + (layout::stroke * 2) + (layout::margin * 2 * 2) + (maxX * layout::stride[X]),
		(layout::margin * 2) + (deser.size() * szPair)
	}, child, sizeof child / sizeof *child);

	_->_parent = scope->_parent;
	
	return _;
}

void dictDel(Dict* dict) {
	objDel(dict->_parent);

	free(dict);
}
