#include <cstdlib>

#include "var.h"

Var* varMk(char* id, void* ptr, unsigned int type) {
	Var* inst = (Var*) malloc(sizeof (Var));

	inst->_id = id;
	inst->_ptr = ptr;
	inst->_type = type;

	return inst;
}

void varDel(Var* inst) {
	free(inst->_id);
	free(inst->_ptr);

	free(inst);
}
