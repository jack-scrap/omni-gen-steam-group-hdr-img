#pragma once

typedef struct Var {
	char* _id;
	void* _ptr;
};

Var* varMk(char* name, void* ptr);
