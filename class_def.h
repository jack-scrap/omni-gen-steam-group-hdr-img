#pragma once

#include <stddef.h>

struct ClassDef {
	size_t sz;
	void (*ctor) (void* self, ...);
	void (*dtor) (void* self);
};
