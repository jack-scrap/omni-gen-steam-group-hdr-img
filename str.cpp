#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>

#include "str.h"
#include "layout.h"

Str* strMk(std::string str, glm::vec3 loc, glm::vec3 rot) {
	Str* _ = (Str*) malloc(sizeof (Str));

	_->_parent = objMk(std::string("glyph/") + str[0], "obj", "solid", true, loc, rot);

	_->_parent->_noChild = str.size() - 1;
	_->_parent->_child = (Obj**) realloc(_->_parent->_child, _->_parent->_noChild * sizeof (Obj*));

	for (int i = 0; i < str.size() - 1; i++) {
		_->_parent->_child[i] = objMk(std::string("glyph/") + str[i + 1], "obj", "solid", true, loc + glm::vec3((i + 1) * (layout::glyph[X] + layout::stroke), 0.0, 0.0), rot);
	}

	return _;
}
