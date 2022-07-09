#include <cstdlib>

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

	inst->_no = deser.size();
	inst->_data = (void**) malloc(inst->_no * sizeof (void*));
	inst->_type = (unsigned int*) malloc(inst->_no * sizeof (unsigned int));

	Obj* child[deser.size() + 1];

	glm::vec2 strideLetter = glm::vec2(layout::item(layout::item(0.0)), layout::item(layout::item(layout::letter[Y])));

	glm::vec2 strideIdx = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	glm::vec2 overhead = glm::vec2(layout::overhead, layout::overhead + strideLetter[Y]);

	// data
	int i = 0;
	GLfloat maxX = 0.0;
	GLfloat accY = 0.0;
	for (const auto& entry : deser.items()) {
		Obj* id;
		switch (entry.value().type()) {
			// scalar
			case nlohmann::json::value_t::number_unsigned: {
				char init = (char) ((int) entry.value());

				Idx* idx = idxMk(0, &init, 1, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

				child[i] = idx->_parent;

				((Idx**) inst->_data)[i] = idx;
				inst->_type[i] = omni::SCALAR;

				if (strideIdx[X] > maxX) {
					maxX = strideIdx[X];
				}

				id = (((Idx**) inst->_data)[i])->_parent->_child[1];

				accY += strideLetter[Y] + strideIdx[Y];

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				switch (entry.value()[0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						CBuff init = util::json::array::array(entry.value());

						Array* array = arrayMk((char*) init.ptr, init.x, entry.key(), X, glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

						child[1 + i] = array->_parent;

						((Array**) inst->_data)[i] = array;
						inst->_type[i] = omni::ARRAY;

						if (layout::scoped(init.x * strideIdx[X]) > maxX) {
							maxX = layout::scoped(init.x * strideIdx[X]);
						}

						id = (((Array**) inst->_data)[i])->_parent->_child[1];

						GLfloat szY = layout::scoped(strideLetter[Y] + strideIdx[Y]);

						accY += szY;

						break;
					}

					// matrix
					case nlohmann::json::value_t::array: {
						switch (entry.value()[0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								CBuff init = util::json::array::matrix(entry.value());

								Array* array = arrayMk((char*) init.ptr, init.x, init.y, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

								child[1 + i] = array->_parent;

								((Array**) inst->_data)[i] = array;
								inst->_type[i] = omni::ARRAY;

								if (layout::scoped(init.x * strideIdx[X]) > maxX) {
									maxX = layout::scoped(init.x * strideIdx[X]);
								}

								id = (((Array**) inst->_data)[i])->_parent->_child[1];

								GLfloat szY = layout::scoped(strideLetter[Y] + (init.y * strideIdx[Y]));

								accY += szY;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								CBuff init = util::json::array::tens(entry.value());

								Array* array = arrayMk((char*) init.ptr, init.x, init.y, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

								child[1 + i] = array->_parent;

								((Array**) inst->_data)[i] = array;
								inst->_type[i] = omni::ARRAY;

								if (layout::scoped(init.x * strideIdx[X]) > maxX) {
									maxX = layout::scoped(init.x * strideIdx[X]);
								}

								glm::vec2 sz = glm::vec2(layout::scoped(0.0), layout::scoped(strideLetter[Y] + (init.y * strideIdx[Y])));

								id = (((Array**) inst->_data)[i])->_parent->_child[1];

								accY += sz[Y];

								break;
							}
						}

						break;
					}
				}

				break;
			}
		}

		// name
		float idWd = 0.0;

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

	// offset
	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->_offset[a] = offset[a];
	}

	return inst;
}

void dictDel(Dict* inst) {
	objDel(inst->_parent);

	free(inst);
}

void dictPush(Dict* inst, std::string key, Cont* byte) {
}

Cont* dictPop(Dict* inst, std::string key) {
}

bool dictEq(Dict* lhs, Dict* rhs) {
	for (int i = 0; i < lhs->_no; i++) {
		switch (lhs->_type[i]) {
			case omni::SCALAR:
				if (!idxEq((Idx*) lhs->_data[i], (Idx*) rhs->_data[i])) {
					return false;
				}

			case omni::ARRAY:
				if (!arrayEq((Array*) lhs->_data[i], (Array*) rhs->_data[i])) {
					return false;
				}
		}
	}

	return true;
}
