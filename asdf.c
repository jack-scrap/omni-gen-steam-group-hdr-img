#include <stdio.h>

#include "asdf.h"

Asdf asdfMk() {
	Asdf* _ = malloc(sizeof (Asdf));

	_->_asdf = 3;

	return *_;
}
