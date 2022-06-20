#include <cstdlib>

#include "var.h"

Var* varMk(char* id, void* ptr, unsigned int type) {
	Var* _ = (Var*) malloc(sizeof (Var));

	_->_id = id;
	_->_ptr = ptr;
	_->_type = type;

	return _;
}

void varDel(Var* inst) {
	free(inst->_id);
	free(inst->_ptr);

	free(inst);
}
