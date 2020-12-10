#pragma once

typedef struct {
	int _asdf;
} Asdf;

Asdf asdfMk();

extern "C" Asdf asdfDraw(Asdf* asdf);
