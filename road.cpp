#include "road.h"
#include "line.h"
#include "lim.h"

Road* roadMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int status, glm::vec3 loc, glm::vec3 rot) {
	Road* _ = (Road*) malloc(sizeof (Road));

	_->_status = status;

	std::string name;
	switch (_->_status) {
		case PASS:
			name = "solid";

			break;

		case HALT:
			name = "solid";

			break;

		case ALERT:
			name = "alert";

			break;
	};

	_->_parent = lineMk(vtc, idc, noPrim, "main", "thick", name, true, loc, rot);
	
	return _;
}

void roadDel(Road* inst) {
	objDel(inst->_parent);

	free(inst);
}
