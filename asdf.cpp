#include <stdio.h>

#include "asdf.h"
#include "hjkl.h"

Asdf asdfMk() {
	Asdf* _ = (Asdf*) malloc(sizeof (Asdf));

	_->_asdf = 3;

	return *_;
}

void set(Asdf* asdf, int hjkl) {
	asdf->_asdf = hjkl;
}
