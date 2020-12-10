#include <cstdlib>

#include "truck.h"

Truck truckMk() {
	// initialize
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_asdf = 3;

	return *_;
}
