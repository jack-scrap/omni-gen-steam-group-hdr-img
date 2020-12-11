#pragma once

typedef struct {
	int _asdf;
} Truck;

Truck* truckMk();

extern "C" void truckDraw(Truck* truck);
