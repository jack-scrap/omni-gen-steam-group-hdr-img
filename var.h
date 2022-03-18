#pragma once

typedef struct {
	char* _id;
	void* _ptr;
	unsigned int _type;
} Var;

Var* varMk(char* id, void* ptr, unsigned int type);

void varDel(Var* var);
