#include <cstdlib>
#include <glm/glm.hpp>

#include "var.h"

Var* varMk(char* id, void* ptr, unsigned int type, glm::vec3 offset) {
	Var* inst = (Var*) malloc(sizeof (Var));

	inst->id = id;
	inst->ptr = ptr;
	inst->type = type;

	return inst;
}

void varDel(Var* inst) {
	free(inst->id);
	free(inst->ptr);

	free(inst);
}
