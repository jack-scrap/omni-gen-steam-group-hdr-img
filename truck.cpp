#include <iostream>

#include "truck.h"

Truck truckMk() {
	Truck _;

	_._asdf = 3;

	return _;
}

Truck truckDraw(Truck* truck) {
	std::cout << truck->_asdf << std::endl;
}
