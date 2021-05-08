#include <cstdlib>

#include "dict.h"
#include "str.h"
#include "layout.h"
#include "border.h"

Dict* dictMk(void* data, unsigned int no, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Dict* _ = (Dict*) malloc(sizeof (Dict));

	Obj* child[1];

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
	}, child, sizeof child / sizeof *child, loc + glm::vec3(0.0), rot);

	_->_parent = scope->_parent;

	return _;
}
