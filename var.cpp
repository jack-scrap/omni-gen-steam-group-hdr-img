#include <cstdlib>

#include "var.h"

Var* varMk(char* id, void* ptr, unsigned int type) {
	Var* _ = (Var*) malloc(sizeof (Var));

	_->_id = id;
	_->_ptr = ptr;
	_->_type = type;

	return _;
}

void varDel(Var* var) {
	free(var->_id);
	free(var->_ptr);

	free(var);
}
