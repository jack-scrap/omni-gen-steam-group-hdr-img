#pragma once

typedef struct {
	char* _id;
	void* _ptr;
} Var;

Var* varMk(char* name, void* ptr);

void varDel(Var* var);
