#include "cone.h"
#include "obj.h"
#include "scn.h"

Cone* coneMk(glm::vec3 loc, glm::vec3 rot) {
	Cone* _ = (Cone*) malloc(sizeof (Cone));

	Obj* bound = objMk(_->_vtx, 3, _->_idx, 1, "bevel/main", "bevel/main", "alert", true, glm::vec3(0.0, 1.408, 0.0));

	Obj* child[] = {
		bound
	};

	_->_parent = objMk("cone", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	pt.push_back(bound);

	return _;
}
