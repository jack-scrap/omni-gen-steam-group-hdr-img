#include <iostream>

#include "asdf.h"
#include "hjkl.h"

int main() {
	Asdf asdf = asdfMk();

	std::cout << asdf._asdf << std::endl;

	hjkl();

	return 0;
}
