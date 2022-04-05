#pragma once

#include <stddef.h>

typedef struct {
	void* _ptr;
	size_t _sz;
} CArr;

typedef struct {
	void* _ptr;
	unsigned int _x;
	unsigned int _y;
	unsigned int _z;
} CBuff;
