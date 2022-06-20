#include "cont.h"
#include "layout.h"
#include "util.h"

Cont* contMk(char c, glm::vec3 loc, glm::vec3 rot) {
	Cont* _ = (Cont*) malloc(sizeof (Cont));

	_->_c = c;

	_->_parent = objMk("container_2x4", "obj", "dir", true, loc, rot);

	if (_->_c >= '!' && _->_c <= '~') {
		_->_parent->_tex = util::tex::spray("glyph/" + util::glyphName(_->_c));
	}

	return _;
}

void contDel(Cont* inst) {
	objDel(inst->_parent);

	free(inst);
}
