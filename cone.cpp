#include "cone.h"
#include "obj.h"

Cone* coneMk(glm::vec3 loc, glm::vec3 rot) {
	Cone* _ = (Cone*) malloc(sizeof (Cone));

	_->_parent = objMk("cone", "obj", "dir", true);

	return _;
}
