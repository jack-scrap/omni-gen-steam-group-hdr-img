#include "cone.h"
#include "obj.h"
#include "scn.h"

GLfloat Cone::_vtx[3] = {
	0.0, 0.0, 0.0
};

GLushort Cone::_idx[1] = {
	0
};

Cone* coneMk(GLfloat init[2][2], glm::vec3 loc, glm::vec3 rot) {
	Cone* _ = (Cone*) malloc(sizeof (Cone));
	
	Obj* child[1];

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_->_bound[x][y] = init[x][y];
		}
	}

	Obj* bound = ptMk(_->_vtx, _->_idx, 1, "main", "bevel/main", "alert", true, glm::vec3(0.0, 1.408, 0.0));

	child[0] = bound;

	_->_parent = objMk("cone", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	return _;
}
