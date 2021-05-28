#include "road.h"
#include "line.h"

Road* roadMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noPrim, bool pass, glm::vec3 loc, glm::vec3 rot) {
	Road* _ = (Road*) malloc(sizeof (Road));

	_->_pass = pass;

	std::string name;
	switch (_->_pass) {
		case Road::PASS:
			name = "solid";

			break;

		case Road::HALT:
			name = "solid";

			break;

		case Road::ALERT:
			name = "alert";

			break;
	};

	_->_parent = lineMk(vtc, noVtc, idc, noPrim, "main", "thick", name, true, loc, rot);
	
	return _;
}
