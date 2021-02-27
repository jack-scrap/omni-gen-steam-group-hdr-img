#include <cstdlib>
#include <iostream>

#include "node.h"

Node* nodeMk(char* data, int sz) {
	Node* _ = (Node*) malloc(sizeof (Node));

	_->_data = data;
	_->_sz = sz;
	_->_child = nullptr;
	_->_noChild = 0;

	return _;
}

Node* nodeMk(char* data, int sz, Node** child, unsigned int noChild) {
	Node* _ = (Node*) malloc(sizeof (Node));

	_->_data = data;
	_->_sz = sz;
	_->_child = child;
	_->_noChild = noChild;

	return _;
}

void nodeDepth(Node* node, unsigned int* depth) {
	(*depth)++;

	if (node->_noChild) {
		if (node->_child[0]) {
			nodeDepth(node->_child[0], depth);
		}
	}
}

void nodeMax(Node* node, unsigned int* sz) {
	if (node->_sz > *sz) {
		*sz = node->_sz;
	}

	for (int i = 0; i < node->_noChild; i++) {
		if (node->_child[i]->_sz > *sz) {
			*sz = node->_child[i]->_sz;
		}
	}
}
