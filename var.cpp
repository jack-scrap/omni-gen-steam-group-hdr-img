#include <cstdlib>

#include "var.h"

Var* varMk(char* id, void* ptr) {
	Var* _ = (Var*) malloc(sizeof (Var));

	_->_id = id;
	_->_ptr = ptr;

	return _;
}

void varDel(Var* var) {
	free(var->_id);
	free(var->_ptr);

	free(var);
}
