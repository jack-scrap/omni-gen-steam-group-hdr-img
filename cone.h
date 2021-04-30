#include <glm/glm.hpp>

#include "obj.h"

typedef struct Cone {
	Obj* _parent;

	GLfloat _vtx[3] = {
		0.0, 0.0, 0.0
	};

	GLushort _idx[1] = {
		0
	};
} Cone;

Cone* coneMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
