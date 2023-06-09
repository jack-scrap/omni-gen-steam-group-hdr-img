#include <cstdlib>
#include <iostream>

#include "node.h"

Node* nodeMk(char* data, int no) {
	Node* inst = (Node*) malloc(sizeof (Node));

	inst->data = data;
	inst->no = no;
	inst->_child = nullptr;
	inst->_noChild = 0;

	return inst;
}

Node* nodeMk(char* data, int no, Node** child, unsigned int noChild) {
	Node* _ = (Node*) malloc(sizeof (Node));

	_->data = data;
	_->no = no;
	_->_child = child;
	_->_noChild = noChild;

	return _;
}

void nodeDel(Node* inst) {
	free(inst->data);

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
	if (inst->no > *no) {
		*no = inst->no;
	}

	for (int i = 0; i < inst->_noChild; i++) {
		if (inst->_child[i]->no > *no) {
			*no = inst->_child[i]->no;
		}
	}
}
