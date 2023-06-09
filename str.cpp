#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>

#include "str.h"
#include "layout.h"
#include "util.h"

Str* strMk(std::string buff, glm::vec3 loc, glm::vec3 rot) {
	Str* inst = (Str*) malloc(sizeof (Str));

	Obj* head = objMk("glyph/" + util::glyphName(buff[0]), "obj", "solid", true);

	GLfloat offset = 0.0;

	GLfloat wd = head->_aabb[X][MAX] - head->_aabb[X][MIN];

	offset += layout::item(wd);

	objDel(head);

	Obj* child[buff.size() - 1];
	for (int i = 0; i < buff.size() - 1; i++) {
		child[i] = objMk("glyph/" + util::glyphName(buff[i + 1]), "obj", "solid", true, glm::vec3(offset, 0.0, 0.0));

		GLfloat wd = child[i]->_aabb[X][MAX] - child[i]->_aabb[X][MIN];

		offset += layout::item(wd);
	}

	inst->_parent = objMk("glyph/" + util::glyphName(buff[0]), "obj", "solid", true, child, sizeof child / sizeof *child, loc);

	return inst;
}

void strDel(Str* inst) {
	objDel(inst->_parent);

	free(inst);
}
