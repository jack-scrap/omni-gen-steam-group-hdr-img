#define GLM_ENABLE_EXPERIMENTAL

#include <cstdlib>
#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "crane.h"
#include "scn.h"
#include "math.h"
#include "state.h"
#include "omni.h"
#include "util.h"
#include "layout.h"

GLfloat Crane::_lightVtc[2 * 2 * 3] = {
	-0.6, -1.0, 0.0,
	0.6, -1.0, 0.0,
	-0.6, 1.0, 0.0,
	0.6, 1.0, 0.0
};

GLushort Crane::_lightIdc[2 * 3] = {
	0, 1, 2,
	2, 1, 3
};

Crane* craneMk(Cont* init, glm::vec3 loc, glm::vec3 rot) {
	Crane* inst = (Crane*) malloc(sizeof (Crane));

	inst->data = nullptr;

	if (init) {
		inst->data = init;
	}

	Obj* child[1 + (2 * 2 * 2 * 2) + (2 * 2) + (2 * 2) + 1];

	// track
	Obj* cont = nullptr;

	Obj* trackChild[] = {
		objMk("crane/head", "obj", "dir", true, &cont, 1),
		objMk("crane/track_back", "obj", "dir", true)
	};

	child[Crane::TRACK] = objMk("crane/track_front", "obj", "dir", true, trackChild, 1 + 1, glm::vec3(0.0, Crane::_rngTrack[MAX], 0.0));

	// data
	if (inst->data) {
		child[Crane::TRACK]->_child[Crane::HEAD]->_child[0] = inst->data->_parent;
	} else {
		child[Crane::TRACK]->_child[Crane::HEAD]->_child[0] = nullptr;
	}

	// wheel
	int w = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					Obj* wheel = objMk("wheel", "obj", "dir", false);

					child[1 + w] = objMk("rim", "obj", "dir", true, &wheel, 1, glm::vec3(((x ? 1 : -1) * 3.0) + (i ? 1 : -1), 1.0, ((z ? 1 : -1) * (10.0 - 0.6)) + ((j ? 1 : -1) * 0.6)), glm::vec3(0.0, z * M_PI, 0.0));

					w++;
				}
			}
		}
	}

	// light
	int l = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[1 + (2 * 2 * 2 * 2) + l] = objMk(Crane::_lightVtc, Crane::_lightIdc, 3 * 2, Mesh::OBJ, "obj", "alert", false, glm::vec3((x ? 1 : -1) * layout::padded(6.0), 1.74, (z ? 1 : -1) * 9.52), glm::vec3(0.0, M_PI / 2, 0.0));

			l++;
		}
	}

	child[1 + (2 * 2 * 2 * 2) + (2 * 2)] = objMk("crane/back", "obj", "dir", true);

	child[1 + (2 * 2 * 2 * 2) + (2 * 2) + 1] = objMk("crane/support_l", "obj", "dir", true);
	child[1 + (2 * 2 * 2 * 2) + (2 * 2) + 2] = objMk("crane/support_r", "obj", "dir", true);

	child[1 + (2 * 2 * 2 * 2) + (2 * 2) + 3] = objMk("crane/body_btm_l", "obj", "dir", true);
	child[1 + (2 * 2 * 2 * 2) + (2 * 2) + 4] = objMk("crane/body_btm_r", "obj", "dir", true);

	inst->_parent = objMk("crane/front", "obj", "dir", true, child, sizeof child / sizeof* child, loc, rot);

	// offset
	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}

	return inst;
}

void craneDel(Crane* inst) {
	if (inst->data) {
		contDel(inst->data);
	}

	objDel(inst->_parent);

	free(inst);
}

void craneAnim(Crane* inst, glm::vec3 loc) {
	for (int i = 0; i < 2 * 2; i++) {
		inst->_parent->_child[1 + (2 * 2 * 2 * 2) + i]->_active = true;
	}

	objMv(inst->_parent, nullptr, loc, glm::vec3(0.0), Crane::_speed);

	for (int i = 0; i < 2 * 2; i++) {
		inst->_parent->_child[1 + (2 * 2 * 2 * 2) + i]->_active = false;
	}
}

void craneZoom(Crane* inst, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	craneAnim(inst, dest);

	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}
}

void cranePan(Crane* inst, float delta) {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, delta));

	glm::vec3 dest = glm::vec3(inst->_parent->_child[Crane::TRACK]->_child[Crane::TRACK]->_model * trans * glm::vec4(glm::vec3(0.0), 1.0));

	if (dest[Z] > Crane::_rngHead[MIN] && dest[Z] < Crane::_rngHead[MAX]) {
		glm::vec3 offset = glm::vec3(0.0, 0.0, delta);

		objMv(inst->_parent->_child[Crane::TRACK]->_child[Crane::TRACK], inst->_parent->_child[Crane::TRACK], offset, glm::vec3(0.0));
	} else {
		omni::err(omni::ERR_MV_RNG, {
			"crane head"
		});
	}

	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}
}

void cranePed(Crane* inst, float delta) {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, delta, 0.0));
	glm::vec3 dest = glm::vec3(inst->_parent->_child[Crane::TRACK]->_acc * trans * glm::vec4(glm::vec3(0.0), 1.0));

	if (dest[Y] > Crane::_rngTrack[MIN] && dest[Y] < Crane::_rngTrack[MAX]) {
		glm::vec3 offset = glm::vec3(0.0, delta, 0.0);

		objMv(inst->_parent->_child[Crane::TRACK], inst->_parent, offset, glm::vec3(0.0));
	} else {
		omni::err(omni::ERR_MV_RNG, {
			"crane head"
		});
	}

	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}
}

void craneGrab(Crane* inst) {
	if (inst->data) {
		/* data */
		for (int i = 0; i < noData; i++) {
			switch (data[i]->type) {
				case omni::IDX: {
					Idx* idx = (Idx*) data[i]->ptr;

					if (util::phys::aabb(glm::vec3(0.0), inst->data->_parent, idx->_parent->_acc, inst->data->_parent->_acc)) {
						idxPush(idx, craneRm(inst));

						return;
					}

					break;
				}

				case omni::ARRAY: {
					Array* array = (Array*) data[i]->ptr;

					for (int y = 0; y < array->y; y++) {
						for (int x = 0; x < array->x; x++) {
							unsigned int i = util::math::idx::array({
								x,
								y
							}, {
								array->x,
								array->y
							});

							Idx* idx = array->data[i];

							if (inst->data && !idx->sz) {
								if (util::phys::aabb(glm::vec3(0.0), inst->data->_parent, idx->_parent->_acc, inst->data->_parent->_acc)) {
									arrayIns(array, x, y, craneRm(inst));

									return;
								}
							}
						}
					}

					break;
				}
			}
		}

		/* vehicle */
		// truck
		for (int i = 0; i < truck.sz / sizeof (Truck*); i++) {
			Truck* truckInst = ((Truck**) truck.ptr)[i];

			Array* array = truckInst->data;

			for (int y = 0; y < array->y; y++) {
				for (int x = 0; x < array->x; x++) {
					unsigned int i = util::math::idx::array({
						x,
						y
					}, {
						array->x,
						array->y
					});

					Idx* idx = array->data[i];

					if (inst->data && !idx->sz) {
						if (util::phys::aabb(glm::vec3(0.0), inst->data->_parent, idx->_parent->_acc, inst->data->_parent->_acc)) {
							arrayIns(array, x, y, craneRm(inst));

							return;
						}
					}
				}
			}
		}

		// cargo ship
		for (int i = 0; i < cargoShip.sz / sizeof (CargoShip*); i++) {
			CargoShip* cargoShipInst = ((CargoShip**) cargoShip.ptr)[i];

			Array* array = cargoShipInst->data;

			for (int y = 0; y < array->y; y++) {
				for (int x = 0; x < array->x; x++) {
					unsigned int i = util::math::idx::array({
						x,
						y
					}, {
						array->x,
						array->y
					});

					Idx* idx = array->data[i];

					if (inst->data && !idx->sz) {
						if (util::phys::aabb(glm::vec3(0.0), inst->data->_parent, idx->_parent->_acc, inst->data->_parent->_acc)) {
							arrayIns(array, x, y, craneRm(inst));

							return;
						}
					}
				}
			}
		}	
	} else {
		/* data */
		for (int i = 0; i < noData; i++) {
			switch (data[i]->type) {
				case omni::IDX: {
					Idx* idx = (Idx*) data[i]->ptr;

					if (idx->sz) {
						if (util::phys::aabb(inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD], idx->data[idx->sz - 1]->_parent, inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_acc, idx->data[idx->sz - 1]->_parent->_acc)) {
							craneInsert(inst, idxPop(idx));

							return;
						}
					}

					break;
				}

				case omni::ARRAY: {
					Array* array = (Array*) data[i]->ptr;

					for (int y = 0; y < array->y; y++) {
						for (int x = 0; x < array->x; x++) {
							unsigned int i = util::math::idx::array({
								x,
								y
							}, {
								array->x,
								array->y
							});

							Idx* idx = array->data[i];

							if (idx->sz) {
								if (util::phys::aabb(inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD], idx->data[idx->sz - 1]->_parent, inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_acc, idx->data[idx->sz - 1]->_parent->_acc)) {
									craneInsert(inst, arrayRm(array, x, y));

									return;
								}
							}
						}
					}

					break;
				}
			}

			/* vehicle */
			// truck
			for (int i = 0; i < truck.sz / sizeof (Truck*); i++) {
				Truck* truckInst = ((Truck**) truck.ptr)[i];

				Array* array = truckInst->data;

				for (int y = 0; y < array->y; y++) {
					for (int x = 0; x < array->x; x++) {
						unsigned int i = util::math::idx::array({
							x,
							y
						}, {
							array->x,
							array->y
						});

						Idx* idx = array->data[i];

						if (idx->sz) {
							Cont* head = idx->data[idx->sz - 1];

							if (util::phys::aabb(inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD], head->_parent, inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_acc, head->_parent->_acc)) {
								craneInsert(inst, idxPop(idx));

								return;
							}
						}
					}
				}
			}

			// cargo ship
			for (int i = 0; i < cargoShip.sz / sizeof (CargoShip*); i++) {
				CargoShip* cargoShipInst = ((CargoShip**) cargoShip.ptr)[i];

				Array* array = cargoShipInst->data;

				for (int y = 0; y < array->y; y++) {
					for (int x = 0; x < array->x; x++) {
						unsigned int i = util::math::idx::array({
							x,
							y
						}, {
							array->x,
							array->y
						});

						Idx* idx = array->data[i];

						if (idx->sz) {
							Cont* head = idx->data[idx->sz - 1];

							if (util::phys::aabb(inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD], head->_parent, inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_acc, head->_parent->_acc)) {
								craneInsert(inst, idxPop(idx));

								return;
							}
						}
					}
				}
			}	
		}
	}
}

void craneInsert(Crane* inst, Cont* byte) {
	inst->data = byte;
	inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_child[0] = inst->data->_parent;

	// transform
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0, -(3.32 + (layout::idx[Y] / 2)), 0.0));

	inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_child[0]->_model = model;

	objAcc(inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD], inst->_parent->_child[Crane::TRACK]->_acc);
}

Cont* craneRm(Crane* inst) {
	Cont* byte = inst->data;

	inst->data = nullptr;
	inst->_parent->_child[Crane::TRACK]->_child[Crane::HEAD]->_child[0] = nullptr;

	return byte;
}
