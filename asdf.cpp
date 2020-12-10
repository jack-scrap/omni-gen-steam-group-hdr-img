#include <iostream>

#include "asdf.h"

Asdf asdfMk() {
	Asdf _;

	_._asdf = 3;

	return _;
}

Asdf asdfDraw(Asdf* asdf) {
	std::cout << asdf->_asdf << std::endl;
}
