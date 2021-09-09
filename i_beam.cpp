#include "i_beam.h"
#include "line.h"
#include "math.h"

GLfloat IBeam::_vtc[3][2][3] = {
	{
		{
			0.0, -1.0, -1.0
		}, {
			0.0, -1.0, 1.0
		}
	}, {
		{
			0.0, 1.0, -1.0
		}, {
			0.0, 1.0, 1.0
		}
	}, {
		{
			0.0, -1.0, 0.0
		}, {
			0.0, 1.0, 0.0
		}
	}
};

GLushort IBeam::_idc[3][2] = {
	{
		0, 1
	}, {
		2, 3
	}, {
		4, 5
	}
};

Obj* iBeamMk(glm::vec3 loc, glm::vec3 rot) {
	Obj* _ = lineMk((GLfloat*) IBeam::_vtc, (GLushort*) IBeam::_idc, sizeof IBeam::_idc / sizeof (GLushort), "main", "beam", "dir", false, loc, rot);

	_->_prog.use();

	GLint uniAxis = glGetUniformLocation(_->_prog._id, "axis");
	GLint uniLn = glGetUniformLocation(_->_prog._id, "ln");

	glUniform1ui(uniAxis, X);
	glUniform1f(uniLn, 3.0);

	_->_prog.unUse();

	return _;
}
