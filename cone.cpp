#include "cone.h"
#include "obj.h"
#include "scn.h"

Cone* coneMk(GLfloat init[2][2], glm::vec3 loc, glm::vec3 rot) {
	Cone* _ = (Cone*) malloc(sizeof (Cone));
	
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_->_bound[x][y] = init[x][y];
		}
	}

	Obj* bound = ptMk(_->_vtx, "bevel/main", "bevel/main", "alert", glm::vec3(0.0, 1.408, 0.0));

	Obj* child[] = {
		bound
	};

	_->_parent = objMk("cone", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	pt.push_back(bound);

	return _;
}
