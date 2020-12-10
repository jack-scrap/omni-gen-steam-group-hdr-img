#pragma once

typedef struct {
	int _asdf;
} Truck;

Truck truckMk();

extern "C" Truck truckDraw(Truck* truck);
