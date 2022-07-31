#include <cstdlib>
#include <iostream>

#include "dict.h"
#include "str.h"
#include "layout.h"
#include "border.h"
#include "idx.h"
#include "scn.h"
#include "omni.h"
#include "util.h"

Dict* dictMk(nlohmann::json deser, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Dict* inst = (Dict*) malloc(sizeof (Dict));

	inst->no = deser.size();
	inst->data = (void**) malloc(inst->no * sizeof (void*));
	inst->type = (unsigned int*) malloc(inst->no * sizeof (unsigned int));

	inst->key = (char**) malloc(inst->no * sizeof (char*));
	int p = 0;
	for (const auto& pair : deser.items()) {
		inst->key[p] = util::json::id(pair.key());

		p++;
	}

	Obj* child[deser.size() + 1];

	glm::vec2 strideLetter = glm::vec2(layout::item(layout::item(0.0)), layout::item(layout::item(layout::letter[Y])));

	glm::vec2 strideIdx = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	glm::vec2 overhead = glm::vec2(layout::overhead, layout::overhead + strideLetter[Y]);

	// data
	int i = 0;
	GLfloat maxX = 0.0;
	GLfloat accY = 0.0;
	for (const auto& entry : deser.items()) {
		int k = dictHash(inst, (char*) entry.key().c_str());

		Obj* id;
		switch (entry.value().type()) {
			// stack
			case nlohmann::json::value_t::number_unsigned: {
				char init = (char) ((int) entry.value());

				Idx* idx = idxMk(0, &init, 1, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

				child[i] = idx->_parent;

				((Idx**) inst->data)[k] = idx;
				inst->type[k] = omni::IDX;

				if (strideIdx[X] > maxX) {
					maxX = strideIdx[X];
				}

				id = (((Idx**) inst->data)[i])->_parent->_child[1];

				accY += strideLetter[Y] + strideIdx[Y];

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				switch (entry.value()[0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						CBuff init = util::json::array::lin(entry.value());

						Array* array = arrayMk((char*) init.ptr, init.x, entry.key(), X, glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

						child[1 + i] = array->_parent;

						((Array**) inst->data)[k] = array;
						inst->type[k] = omni::ARRAY;

						if (layout::scoped(init.x * strideIdx[X]) > maxX) {
							maxX = layout::scoped(init.x * strideIdx[X]);
						}

						id = (((Array**) inst->data)[i])->_parent->_child[1];

						GLfloat szY = layout::scoped(strideLetter[Y] + strideIdx[Y]);

						accY += szY;

						break;
					}

					// 2D
					case nlohmann::json::value_t::array: {
						CBuff init = util::json::array::matrix(entry.value());

						Array* array = arrayMk((char*) init.ptr, init.x, init.y, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

						child[1 + i] = array->_parent;

						((Array**) inst->data)[k] = array;
						inst->type[k] = omni::ARRAY;

						if (layout::scoped(init.x * strideIdx[X]) > maxX) {
							maxX = layout::scoped(init.x * strideIdx[X]);
						}

						id = (((Array**) inst->data)[i])->_parent->_child[1];

						GLfloat szY = layout::scoped(strideLetter[Y] + (init.y * strideIdx[Y]));

						accY += szY;

						break;
					}
				}

				break;
			}

			// dictionary
			case nlohmann::json::value_t::object: {
				Dict* dict = dictMk(entry.value(), name, loc, rot);

				child[1 + i] = dict->_parent;

				((Dict**) inst->data)[k] = dict;
				inst->type[k] = omni::DICT;

				break;
			}																			 
		}

		// name
		GLfloat idWd = 0.0;

		idWd += layout::item(id->_aabb[X][MAX] - id->_aabb[X][MIN]);

		for (int i = 0; i < id->_noChild; i++) {
			idWd += layout::item(id->_child[i]->_aabb[X][MAX] - id->_child[i]->_aabb[X][MIN]);
		}

		if (idWd > maxX) {
			maxX = idWd;
		}

		i++;
	}

	// identifier
	child[deser.size()] = nullptr;

	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[deser.size()] = id->_parent;
	}

	// scope
	Border* scope = borderMk({
		maxX,
		accY
	}, child, sizeof child / sizeof *child);

	inst->_parent = scope->_parent;

	return inst;
}

Dict* dictMk(Node* node, std::string name, glm::vec3 loc, glm::vec3 rot) {
}

void dictDel(Dict* inst) {
	objDel(inst->_parent);

	free(inst);
}

int dictHash(Dict* inst, char* key) {
	int i = 0;
	char c = key[i];
	int sum = 0;
	while (c != '\0') {
		sum += c;

		i++;
		c = key[i];
	}

	return sum % inst->no;
}

void dictIns(Dict* inst, std::string key, unsigned int x, unsigned int y, Cont* byte) {
	int k = dictHash(inst, (char*) key.c_str());

	switch (inst->type[k]) {
		case omni::IDX:
			idxPush((Idx*) inst->data[k], byte);

			break;

		case omni::ARRAY:
			arrayIns((Array*) inst->data[k], x, y, byte);

			break;
	}
}

Cont* dictRm(Dict* inst, std::string key, unsigned int x, unsigned int y) {
	Cont* byte = nullptr;

	int k = dictHash(inst, (char*) key.c_str());

	switch (inst->type[k]) {
		case omni::IDX:
			byte = idxPop((Idx*) inst->data[k]);

			break;

		case omni::ARRAY:
			byte = arrayRm((Array*) inst->data[k], 0, 0);

			break;
	}

	return byte;
}

bool dictEq(Dict* lhs, Dict* rhs) {
	for (int i = 0; i < lhs->no; i++) {
		switch (lhs->type[i]) {
			case omni::IDX:
				if (!idxEq((Idx*) lhs->data[i], (Idx*) rhs->data[i])) {
					return false;
				}

			case omni::ARRAY:
				if (!arrayEq((Array*) lhs->data[i], (Array*) rhs->data[i])) {
					return false;
				}
		}
	}

	return true;
}
