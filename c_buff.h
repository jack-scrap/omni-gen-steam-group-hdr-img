#pragma once

#include <stddef.h>

typedef struct {
	void* ptr;
	size_t sz;
} CArr;

typedef struct {
	void* ptr;
	unsigned int x;
	unsigned int y;
	unsigned int z;
} CBuff;
