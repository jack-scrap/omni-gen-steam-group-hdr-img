#include "cont.h"
#include "layout.h"
#include "util.h"

Cont* contMk(char c, glm::vec3 loc, glm::vec3 rot) {
	Cont* inst = (Cont*) malloc(sizeof (Cont));

	inst->_c = c;

	inst->_parent = objMk("container_2x4", "obj", "dir", true, loc, rot);

	if (inst->_c >= '!' && inst->_c <= '~') {
		inst->_parent->_tex = util::tex::spray("glyph/" + util::glyphName(std::tolower(inst->_c)));
	}

	return inst;
}

void contDel(Cont* inst) {
	objDel(inst->_parent);

	free(inst);
}
