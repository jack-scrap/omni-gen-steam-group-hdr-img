#pragma once

#include <cstdlib>

typedef struct {
	int _asdf;
} Asdf;

extern "C" Asdf asdfMk();

extern "C" void set(Asdf* asdf, int hjkl);
