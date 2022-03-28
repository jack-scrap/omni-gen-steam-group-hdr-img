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
	Dict* _ = (Dict*) malloc(sizeof (Dict));

	_->_no = deser.size();
	_->_data = (void**) malloc(_->_no * sizeof (void*));
	_->_type = (unsigned int*) malloc(_->_no * sizeof (unsigned int));

	Obj* child[1 + deser.size()];

	glm::vec2 strideLetter = glm::vec2(layout::item(layout::item(0.0)), layout::item(layout::item(layout::letter[Y])));

	glm::vec2 strideIdx = glm::vec2(layout::item(layout::bordered(layout::idx[X])), layout::item(layout::bordered(layout::idx[Z])));

	glm::vec2 overhead = glm::vec2(layout::overhead, layout::overhead + strideLetter[Y]);

	// data
	int i = 0;
	GLfloat maxX = 0.0;
	GLfloat accY = 0.0;
	for (const auto& entry : deser.items()) {
		switch (entry.value().type()) {
			// scalar
			case nlohmann::json::value_t::number_unsigned: {
				char init = (char) ((int) entry.value());

				Idx* idx = idxMk(0, &init, 1, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

				child[1 + i] = idx->_parent;

				((Idx**) _->_data)[i] = idx;
				_->_type[i] = omni::SCALAR;

				if (strideIdx[X] > maxX) {
					maxX = strideIdx[X];
				}

				accY += strideLetter[Y] + strideIdx[Y];

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				switch (entry.value()[0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						CBuff init = util::json::array::array(entry.value());

						Array* array = arrayMk((char*) init._ptr, init._x, entry.key(), X, glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

						child[1 + i] = array->_parent;

						((Array**) _->_data)[i] = array;
						_->_type[i] = omni::ARRAY;

						if (layout::bordered(init._x * strideIdx[X]) > maxX) {
							maxX = layout::bordered(init._x * strideIdx[X]);
						}

						GLfloat szY = layout::bordered(strideLetter[Y] + strideIdx[Y]);

						accY += szY;

						break;
					}

					// matrix
					case nlohmann::json::value_t::array: {
						switch (entry.value()[0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								CBuff init = util::json::array::matr(entry.value());

								Array* array = arrayMk((char*) init._ptr, init._x, init._y, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

								child[1 + i] = array->_parent;

								((Array**) _->_data)[i] = array;
								_->_type[i] = omni::ARRAY;

								if (layout::bordered(init._x * strideIdx[X]) > maxX) {
									maxX = layout::bordered(init._x * strideIdx[X]);
								}

								GLfloat szY = layout::bordered(strideLetter[Y] + (init._y * strideIdx[Y]));

								accY += szY;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								CBuff init = util::json::array::tens(entry.value());

								Array* array = arrayMk((char*) init._ptr, init._x, init._y, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

								child[1 + i] = array->_parent;

								((Array**) _->_data)[i] = array;
								_->_type[i] = omni::ARRAY;

								if (layout::bordered(init._x * strideIdx[X]) > maxX) {
									maxX = layout::bordered(init._x * strideIdx[X]);
								}

								glm::vec2 sz = glm::vec2(layout::bordered(0.0), layout::bordered(strideLetter[Y] + (init._y * strideIdx[Y])));

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

		i++;
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	} else {
		child[0] = nullptr;
	}

	// scope
	Border* scope = borderMk({
		maxX,
		accY
	}, child, sizeof child / sizeof *child);

	_->_parent = scope->_parent;

	return _;
}

void dictDel(Dict* dict) {
	objDel(dict->_parent);

	free(dict);
}

bool dictEq(Dict* lhs, Dict* rhs) {
	bool _ = true;

	for (int i = 0; i < lhs->_no; i++) {
		if (!_) {
			break;
		}

		switch (lhs->_type[i]) {
			case omni::SCALAR:
				if (!idxEq((Idx*) lhs->_data[i], (Idx*) rhs->_data[i])) {
					_ = false;
				}

				break;

			case omni::ARRAY:
				if (!arrayEq((Array*) lhs->_data[i], (Array*) rhs->_data[i])) {
					_ = false;
				}

				break;
		}
	}

	return _;
}
