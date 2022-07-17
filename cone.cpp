#include "cone.h"
#include "obj.h"
#include "scn.h"

GLfloat Cone::_vtx[3] = {
	0.0, 0.0, 0.0
};

GLushort Cone::_idx[1] = {
	0
};

Cone* coneMk(GLfloat init[2][2], glm::vec3 loc, glm::vec3 rot) {
	Cone* inst = (Cone*) malloc(sizeof (Cone));
	
	Obj* child[1];

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			inst->bound[x][y] = init[x][y];
		}
	}

	Obj* bound = ptMk(inst->_vtx, inst->_idx, 1, "main", "bevel/main", "alert", true, glm::vec3(0.0, 1.408, 0.0));

	bound->_prog.use();

	bound->_noUni += 2;
	bound->_uni = (GLint*) realloc(bound->_uni, bound->_noUni * sizeof (bound->_noUni));

	bound->_uni[5] = glGetUniformLocation(bound->_prog._id, "boundX");
	bound->_uni[6] = glGetUniformLocation(bound->_prog._id, "boundZ");

	glUniform2fv(bound->_uni[5], 1, inst->bound[X]);
	glUniform2fv(bound->_uni[6], 1, inst->bound[Y]);

	bound->_prog.unUse();

	child[0] = bound;

	inst->_parent = objMk("cone", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	// offset
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = loc[a];
	}

	return inst;
}

void coneDel(Cone* inst) {
	objDel(inst->_parent);

	free(inst);
}
