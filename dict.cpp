#include <cstdlib>

#include "dict.h"
#include "str.h"
#include "layout.h"
#include "border.h"
#include "idx.h"
#include "scn.h"
#include "omni.h"

Dict* dictMk(nlohmann::json deser, glm::vec3 loc, glm::vec3 rot) {
	Dict* _ = (Dict*) malloc(sizeof (Dict));

	for (const auto& pair : deser) {
		std::cout << pair << std::endl;
	}

	// scope
	Border* scope = borderMk({
		1.0,
		1.0
	});

	_->_parent = scope->_parent;
	
	return _;
}
