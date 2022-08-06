#pragma once

typedef struct {
	char* id;
	void* ptr;
	unsigned int type;

	float offset[3];
} Var;

Var* varMk(char* id, void* ptr, unsigned int type, glm::vec3 offset = glm::vec3(0.0));

void varDel(Var* inst);
