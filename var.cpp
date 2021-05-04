#include <cstdlib>

#include "var.h"

Var* varMk(char* id, void* ptr) {
	Var* _ = (Var*) malloc(sizeof (Var));

	_->_id = id;
	_->_ptr = ptr;

	return _;
}
