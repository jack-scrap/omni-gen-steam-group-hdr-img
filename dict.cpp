#include <cstdlib>

#include "dict.h"
#include "str.h"
#include "layout.h"
#include "border.h"
#include "idx.h"
#include "scn.h"

Dict* dictMk(Var** data, unsigned int* type, unsigned int no, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Dict* _ = (Dict*) malloc(sizeof (Dict));
	
	Obj* child[1];

	for (int i = 0; i < no; i++) {
		Var* var = (Var*) data[i];

		switch (type[i]) {
			case SCALAR: {
			 	Idx* idx = (Idx*) var->_ptr;

				break;
			}

			case ARRAY: {
				Arr* arr = (Arr*) var->_ptr;

				break;
			}
		}
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	} else {
		child[0] = nullptr;
	}

	// scope
	Border* scope = borderMk({
		1.0,
		1.0
	}, child, sizeof child / sizeof *child, loc, rot);

	_->_parent = scope->_parent;

	return _;
}
