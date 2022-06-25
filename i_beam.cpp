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
	Obj* inst = lineMk(IBeam::_vtc, IBeam::_idc, sizeof IBeam::_idc / sizeof (GLushort), "main", "beam", "dir", false, loc, rot);

	inst->_prog.use();

	inst->_uni[IBeam::AXIS] = glGetUniformLocation(inst->_prog._id, "axis");
	inst->_uni[IBeam::LN] = glGetUniformLocation(inst->_prog._id, "ln");

	glUniform1ui(inst->_uni[IBeam::AXIS], axis);
	glUniform1f(inst->_uni[IBeam::LN], ln);

	inst->_prog.unUse();

	return inst;
}
