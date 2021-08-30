#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "console.h"
#include "omni.h"
#include "mesh.h"
#include "scn.h"
#include "disp.h"
#include "util.h"
#include "col.h"
#include "stb_image.h"
#include "layout.h"
#include "math.h"

Console::Console() :
	_prog("text", "text") {
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 2 * sizeof (GLfloat), _vtc, GL_STATIC_DRAW);

		glGenBuffers(1, &_stbo);
		glBindBuffer(GL_ARRAY_BUFFER, _stbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 2 * sizeof (GLfloat), _st, GL_STATIC_DRAW);

		glGenBuffers(1, &_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof (GLushort), _idc, GL_STATIC_DRAW);

		_prog.use();

		// attribute
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		_attrPos = glGetAttribLocation(_prog._id, "pos");
		glVertexAttribPointer(_attrPos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attrPos);

		glBindBuffer(GL_ARRAY_BUFFER, _stbo);
		_attrSt = glGetAttribLocation(_prog._id, "st");
		glVertexAttribPointer(_attrSt, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attrSt);

		// texture
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state::console[X] * layout::dim[X], state::console[Y] * layout::dim[Y], 0, GL_RGB, GL_UNSIGNED_BYTE, _data);

		glGenerateMipmap(GL_TEXTURE_2D);

		for (int c = 0; c < _buff.size(); c++) {
			print(_buff[c], false, {
				c,
				0
			});
		}

		_prog.unUse();
	}

void Console::print(char c, bool b, Coord st) {
	unsigned int
		idx[2] = {
			c % 16,
			c / 16
		},

		sz = layout::map[X] * layout::map[Y] * layout::dim[X] * layout::dim[Y],
		stride[2] = {
			layout::dim[X],
			layout::map[X] * layout::dim[X] * layout::dim[Y]
		};

	glTexSubImage2D(GL_TEXTURE_2D, 0, st._x * layout::dim[X], st._y * layout::dim[Y], layout::dim[X], layout::dim[Y], GL_BGR, GL_UNSIGNED_BYTE, &(((char*) disp->_map->pixels)[((b * sz) + (idx[Y] * stride[Y]) + (idx[X] * stride[X])) * 3]));
}

void Console::draw() {
	glBindVertexArray(_vao);
	_prog.use();

	glBindTexture(GL_TEXTURE_2D, _tex);

	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	_prog.unUse();
	glBindVertexArray(0);
}

unsigned int Console::idxStatic(Coord st, unsigned int bound[2]) {
	return (st._y * bound[X]) + st._x;
}
