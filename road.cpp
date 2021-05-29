#include "road.h"
#include "line.h"
#include "lim.h"

Road* roadMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noPrim, unsigned int status, glm::vec3 loc, glm::vec3 rot) {
	Road* _ = (Road*) malloc(sizeof (Road));

	_->_status = status;

	std::string name;
	switch (_->_status) {
		case Lim::PASS:
			name = "solid";

			break;

		case Lim::HALT:
			name = "solid";

			break;

		case Lim::ALERT:
			name = "alert";

			break;
	};

	_->_parent = lineMk(vtc, idc, noPrim, "main", "thick", name, true, loc, rot);
	
	return _;
}
