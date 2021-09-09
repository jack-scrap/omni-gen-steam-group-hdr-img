#pragma once

#include <stddef.h>

#include "var.h"

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

typedef struct {
	Var** _ptr;
	unsigned int* _type;
	unsigned int _no;
} Scope;
