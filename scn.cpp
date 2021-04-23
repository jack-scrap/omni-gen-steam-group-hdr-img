#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

#include "scn.h"
#include "layout.h"
#include "col.h"
#include "crane.h"
#include "node.h"
#include "arr.h"
#include "lim.h"
#include "cone.h"
#include "omni.h"

Arr* data;
char* rhs;
unsigned int rhsSz;
bool eq = false;

Attr attr;

void* attrGet() {
	return &attr;
}

void scn::init(unsigned int stage, unsigned int lvl) {
	nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + omni::stage[stage] + "/" + std::to_string(lvl) + ".json"));

	// vehicles
	for (void* _ : vehicle) {
		free(_);
	}
	vehicle.clear();

	// drawable objects
	for (void* _ : mesh) {
		free(_);
	}
	mesh.clear();

	for (const auto& entry : serial["vehicle"]) {
		if (entry["name"] == "crane") {
			Crane* crane = craneMk(glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			vehicle.push_back(crane);
			mesh.push_back(crane->_parent);
		}

		if (entry["name"] == "truck") {
			Truck* truck = truckMk(glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			vehicle.push_back(truck);
			mesh.push_back(truck->_parent);

			GLfloat vtx[3] = {
				0.0, 0.0, 0.0
			};

			GLushort idx[Y] = {
				0
			};

			Obj* bed = objMk(vtx, 1, idx, 1, "bevel/main", "bed", "tex", false, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]) + glm::vec3(2.0, 1.0 + 0.8, 0.0), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]) + glm::vec3(0.0, M_PI / 2, 0.0));
			Obj* outer = objMk(vtx, 1, idx, 1, "bevel/main", "outer", "dir", true, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]) + glm::vec3(2.0, 1.0 + 0.8, 0.0), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]) + glm::vec3(0.0, M_PI / 2, 0.0));

			pt.push_back(bed);
			pt.push_back(outer);

			GLfloat vtc[(2 * 2 * 2) * 3];
			int i = 0;
			for (int z = 0; z < 2; z++) {
				for (int y = 0; y < 2; y++) {
					for (int x = 0; x < 2; x++) {
						vtc[i] = x ? 1 : -1;
						vtc[i + 1] = y ? 1 : -1;
						vtc[i + 2] = z ? 1 : -1;

						i += 3;
					}
				}
			}

			GLuint idc[3 * 2 * 3 * 2] = {
				0, 1, 2,
				2, 1, 3,

				4, 5, 6,
				6, 5, 7,

				0, 4, 1,
				1, 4, 5,

				2, 6, 3,
				3, 6, 7,

				0, 4, 2,
				2, 4, 6,

				1, 5, 3,
				3, 5, 7
			};
		}
	}

	// data
	for (const auto& pair : serial["data"].items()) {
		// 1D
		if (serial["data"]["state"][0].type() == nlohmann::json::value_t::number_unsigned) {
			char* init = (char*) malloc(0);
			unsigned int sz = 0;

			for (const auto& item : serial["data"][pair.key()]) {
				if (item.type() == nlohmann::json::value_t::number_unsigned) {
					sz++;

					init = (char*) realloc(init, sz * sizeof (char));
					init[sz - 1] = (char) ((int) item);
				}
			}

			data = arrMk(init, sz, pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));
			attr = {
				init,
				data->_x,
				data->_y, {
					data->_loc[0],
					data->_loc[1],
					data->_loc[2]
				}
			};
		}

		// 2D
		if (serial["data"]["state"][0].type() == nlohmann::json::value_t::array) {
			char* init = (char*) malloc(0);
			unsigned int x = 0;
			unsigned int y = 0;

			unsigned int max = 0;
			for (const auto& item : serial["data"][pair.key()]) {
				x = 0;

				for (const auto& byte : item) {
					x++;

					if (x > max) {
						max = x;
					}

					unsigned int sz = ((y * max) + x);

					init = (char*) realloc(init, sz * sizeof (char));
					init[sz - 1] = (char) ((int) byte);
				}

				y++;
			}

			data = arrMk(init, x, y, pair.key(), glm::vec3(0.0, 0.0, -(((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2)) * y)));
			rhsSz = x * y;
		}
	}

	for (const auto& entry : serial["rhs"]) {
		rhs = (char*) malloc(0);

		if (entry.type() == nlohmann::json::value_t::number_unsigned) {
			rhsSz++;

			rhs = (char*) realloc(rhs, rhsSz);
			rhs[rhsSz - 1] = (char) ((int) entry);
		}

		if (entry.type() == nlohmann::json::value_t::array) {
			for (const auto& byte : entry) {
				rhsSz++;

				rhs = (char*) realloc(rhs, rhsSz);
				rhs[rhsSz - 1] = (char) ((int) entry);
			}
		}
	}

	// object
	for (const auto& entry : serial["obj"]) {
		Obj* obj = objMk(entry["name"], "obj", "dir", true, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

		mesh.push_back(obj);
	}

	// bound
	for (const auto& entry : serial["bound"].items()) {
		if (entry.key() == "rng") {
			for (const auto& rng : entry.value()) {
				for (const auto bound : rng) {
					Lim* lim = limMk(glm::vec3(bound["loc"][X], bound["loc"][Y], bound["loc"][Z]), glm::vec3(bound["rot"][X], bound["rot"][Y], bound["rot"][Z]));

					mesh.push_back(lim->_parent);
				}
			}
		}

		if (entry.key() == "cone") {
			for (const auto& entry : entry.value()) {
				Cone* cone = coneMk(glm::vec3(entry[X], entry[Y], entry[Z]));

				mesh.push_back(cone->_parent);

				GLfloat vtx[3] = {
					0.0, 0.0, 0.0
				};

				GLushort idx[1] = {
					0
				};

				pt.push_back(objMk(vtx, 3, idx, 1, "bevel/main", "bevel/main", "alert", true, glm::vec3(entry[X], entry[Y], entry[Z]) + glm::vec3(0.0, 1.408, 0.0)));
			}
		}
	}

	mesh.push_back(data->_parent);
}

std::vector<void*> vehicle;
extern "C" void** vehicleGet() {
	return &vehicle[0];
}

std::vector<Obj*> mesh;
std::vector<Obj*> pt;

extern "C" void* dataGet() {
	return data;
}

extern "C" void* rhsGet() {
	return rhs;
}
