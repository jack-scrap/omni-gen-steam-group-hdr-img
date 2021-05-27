#include <iostream>

#include "omni.h"

void omni::err(std::string msg) {
	std::cout << "Error: " << msg << std::endl;
}

void omni::assertion(bool cond, std::string msg) {
	if (!cond) {
		std::cout << "Assertion failed: " << msg << std::endl;

		exit(1);
	}
}
