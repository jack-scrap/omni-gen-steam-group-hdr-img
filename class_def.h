#pragma once

#include <stdlib.h>

typedef struct {
	size_t sz;
	void* (*ctor) (void* self, ...);
	void* (*dtor) (void* self);
} ClassDef;
