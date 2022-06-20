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

void nodeDel(Node* inst) {
	free(inst->_data);

	for (int i = 0; i < inst->_noChild; i++) {
		nodeDel(inst->_child[i]);
	}
	free(inst->_child);

	free(inst);
}

void nodeDepth(Node* inst, unsigned int* depth) {
	(*depth)++;

	if (inst->_noChild) {
		if (inst->_child[0]) {
			nodeDepth(inst->_child[0], depth);
		}
	}
}

void nodeMax(Node* inst, unsigned int* no) {
	if (inst->_no > *no) {
		*no = inst->_no;
	}

	for (int i = 0; i < inst->_noChild; i++) {
		if (inst->_child[i]->_no > *no) {
			*no = inst->_child[i]->_no;
		}
	}
}
