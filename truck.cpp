#include <iostream>

#include "truck.h"

Truck* truckMk() {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_asdf = 3;

	return _;
}

void truckDraw(Truck* truck) {
	std::cout << truck->_asdf << std::endl;
}
