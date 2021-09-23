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
	
	_->_no = 1;
	_->_data = (void**) malloc(_->_no * sizeof (void*));
	_->_type = (unsigned int*) malloc(_->_no * sizeof (unsigned int));

	Obj* child[deser.size()];

	glm::vec2 strideLetter = layout::margined(layout::margined({
		0.0,
		layout::letter[Y]
	}));

	glm::vec2 strideIdx = layout::bordered({
		layout::idx[X],
		layout::idx[Z]
	});

	glm::vec2 overhead = glm::vec2(layout::overhead, layout::overhead + strideLetter[Y]);

	// data
	int i = 0;
	for (const auto& entry : deser.items()) {
		// scalar
		if (entry.value().type() == nlohmann::json::value_t::number_integer) {
			char init = (int) entry.value();

			glm::vec2 var = glm::vec2(
				strideIdx[X],
				strideLetter[Y] + strideIdx[Y]
			);

			Idx* _ = idxMk(0, &init, 1, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, i * var[Y]));

			child[i] = _->_parent;
		}

		// array
		if (entry.value().type() == nlohmann::json::value_t::array) {
			CBuff init = util::json::array::array(entry.value());

			glm::vec2 var = glm::vec2(
				strideIdx[X],
				strideLetter[Y] + strideIdx[Y]
			);

			Array* _ = arrayMk((char*) init._ptr, init._x, init._y, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, i * var[Y]));

			child[i] = _->_parent;
		}

		i++;
	}

	// scope
	Border* scope = borderMk(layout::margined({
		20.0,
		20.0
	}), child, sizeof child / sizeof *child);

	_->_parent = scope->_parent;

	return _;
}

void dictDel(Dict* dict) {
	objDel(dict->_parent);

	free(dict);
}

bool dictEq(Dict* lhs, Dict* rhs) {
	bool _ = true;

	for (int i = 0; i < lhs->_no; i++) {
		if (!_) {
			break;
		}

		switch (lhs->_type[i]) {
			case omni::SCALAR:
				if (!idxEq((Idx*) lhs->_data[i], (Idx*) rhs->_data[i])) {
					_ = false;
				}

				break;

			case omni::ARRAY:
				if (!arrayEq((Array*) lhs->_data[i], (Array*) rhs->_data[i])) {
					_ = false;
				}

				break;
		}
	}

	return _;
}
