#include "i_beam.h"
#include "line.h"
#include "math.h"

GLfloat IBeam::_vtc[3 * 2 * 3] = {
	0.0, -1.0, -1.0,
	0.0, -1.0, 1.0,
	0.0, 1.0, -1.0,
	0.0, 1.0, 1.0,
	0.0, -1.0, 0.0,
	0.0, 1.0, 0.0
};

GLushort IBeam::_idc[3 * 2] = {
	0, 1,
	2, 3,
	4, 5
};

Obj* iBeamMk(GLfloat ln, unsigned int axis, glm::vec3 loc, glm::vec3 rot) {
	Obj* _ = lineMk(IBeam::_vtc, IBeam::_idc, sizeof IBeam::_idc / sizeof (GLushort), "main", "beam", "dir", false, loc, rot);

	_->_prog.use();

	_->_uni[IBeam::AXIS] = glGetUniformLocation(_->_prog._id, "axis");
	_->_uni[IBeam::LN] = glGetUniformLocation(_->_prog._id, "ln");

	glUniform1ui(_->_uni[IBeam::AXIS], axis);
	glUniform1f(_->_uni[IBeam::LN], ln);

	_->_prog.unUse();

	return _;
}
