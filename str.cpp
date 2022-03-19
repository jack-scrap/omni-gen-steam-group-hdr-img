#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>

#include "str.h"
#include "layout.h"

Str* strMk(std::string buff, glm::vec3 loc, glm::vec3 rot) {
	Str* _ = (Str*) malloc(sizeof (Str));

	Obj* head = objMk(std::string("glyph/") + buff[0], "obj", "solid", true);

	float headWd = head->_aabb[X][MAX] - head->_aabb[X][MIN];

	float offset = layout::padded(headWd);

	objDel(head);

	Obj* child[buff.size() - 1];
	for (int i = 0; i < buff.size() - 1; i++) {
		child[i] = objMk(std::string("glyph/") + buff[i + 1], "obj", "solid", true, glm::vec3(offset, 0.0, 0.0));

		float wd = child[i]->_aabb[X][MAX] - child[i]->_aabb[X][MIN];

		offset += layout::padded(wd);
	}

	_->_parent = objMk(std::string("glyph/") + buff[0], "obj", "solid", true, child, sizeof child / sizeof *child, loc);

	return _;
}

void strDel(Str* str) {
	objDel(str->_parent);

	free(str);
}
