#include <cstdlib>
#include <iostream>

#include "node.h"

Node* nodeMk(char* data, int no) {
	Node* _ = (Node*) malloc(sizeof (Node));

	_->_data = data;
	_->_no = no;
	_->_child = nullptr;
	_->_noChild = 0;

	return _;
}

Node* nodeMk(char* data, int no, Node** child, unsigned int noChild) {
	Node* _ = (Node*) malloc(sizeof (Node));

	_->_data = data;
	_->_no = no;
	_->_child = child;
	_->_noChild = noChild;

	return _;
}

void nodeDel(Node* node) {
	free(node->_data);

	for (int i = 0; i < node->_noChild; i++) {
		nodeDel(node->_child[i]);
	}
	free(node->_child);

	free(node);
}

void nodeDepth(Node* node, unsigned int* depth) {
	(*depth)++;

	if (node->_noChild) {
		if (node->_child[0]) {
			nodeDepth(node->_child[0], depth);
		}
	}
}

void nodeMax(Node* node, unsigned int* no) {
	if (node->_no > *no) {
		*no = node->_no;
	}

	for (int i = 0; i < node->_noChild; i++) {
		if (node->_child[i]->_no > *no) {
			*no = node->_child[i]->_no;
		}
	}
}
