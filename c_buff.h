#pragma once

typedef struct {
	void* _ptr;
	size_t _sz;
} CArr;

typedef struct {
	void* _ptr;
	unsigned int
		_x,
		_y,
		_z;
} CBuff;
