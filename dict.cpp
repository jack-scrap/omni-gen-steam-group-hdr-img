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

	glm::vec2 stride = layout::bordered({
		layout::idx[X],
		layout::idx[Z]
	});

	// data
	int i = 0;
	for (const auto& entry : deser.items()) {
		char init = (int) entry.value();

		Idx* _ = idxMk(0, &init, 1, entry.key(), glm::vec3(layout::overhead, 0.0, layout::overhead));

		child[i] = _->_parent;
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
