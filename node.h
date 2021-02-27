#pragma once

#include <string>
#include <iostream>

#include "obj.h"

typedef struct Node {
	char* _data;
	int _sz;

	Node** _child;
	unsigned int _noChild;
} Node;

Node* nodeMk(char* data, int sz);

Node* nodeMk(char* data, int sz, Node** child, unsigned int noChild);

void nodeDepth(Node* node, unsigned int* depth);

void nodeMax(Node* node, unsigned int* sz);
