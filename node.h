#pragma once

#include <string>
#include <iostream>

#include "obj.h"

typedef struct Node {
	char* data;
	int no;

	Node** _child;
	unsigned int _noChild;
} Node;

Node* nodeMk(char* data, int no);

Node* nodeMk(char* data, int no, Node** child, unsigned int noChild);

void nodeDel(Node* inst);

void nodeDepth(Node* inst, unsigned int* depth);

void nodeMax(Node* inst, unsigned int* no);
