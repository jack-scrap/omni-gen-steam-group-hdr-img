#include "road.h"
#include "line.h"
#include "lim.h"

Road* roadMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int status, glm::vec3 loc, glm::vec3 rot) {
	Road* inst = (Road*) malloc(sizeof (Road));

	inst->_status = status;

	std::string name;
	switch (inst->_status) {
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

	inst->_parent = lineMk(vtc, idc, noPrim, "main", "thick", name, true, loc, rot);
	
	return inst;
}

void roadDel(Road* inst) {
	objDel(inst->_parent);

	free(inst);
}
