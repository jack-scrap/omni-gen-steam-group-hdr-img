#include <GL/glew.h>
#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;

	GLfloat _vtx[3] = {
		0.0, 0.0, 0.0
	};

	GLushort _idx[1] = {
		0
	};

	GLfloat _bound[2][2];
} Cone;

Cone* coneMk(GLfloat init[2][2], glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
