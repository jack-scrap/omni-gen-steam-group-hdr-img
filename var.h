#pragma once

typedef struct {
	char* id;
	void* ptr;
	unsigned int type;
} Var;

Var* varMk(char* id, void* ptr, unsigned int type);

void varDel(Var* inst);
