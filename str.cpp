#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>

#include "str.h"
#include "layout.h"
#include "util.h"

Str* strMk(std::string buff, glm::vec3 loc, glm::vec3 rot) {
	Str* _ = (Str*) malloc(sizeof (Str));

	Obj* head = objMk("glyph" + util::fs::path::sep + buff[0], "obj", "solid", true);

	float wd = head->_aabb[X][MAX] - head->_aabb[X][MIN];

	float offset = layout::padded(wd);

	objDel(head);

	Obj* child[buff.size() - 1];
	for (int i = 0; i < buff.size() - 1; i++) {
		child[i] = objMk("glyph" + util::fs::path::sep + buff[i + 1], "obj", "solid", true, glm::vec3(offset, 0.0, 0.0));

		float wd = child[i]->_aabb[X][MAX] - child[i]->_aabb[X][MIN];

		offset += layout::padded(wd);
	}

	_->_parent = objMk("glyph" + util::fs::path::sep + buff[0], "obj", "solid", true, child, sizeof child / sizeof *child, loc);

	return _;
}

void strDel(Str* str) {
	objDel(str->_parent);

	free(str);
}
