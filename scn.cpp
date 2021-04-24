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
#include "data.h"
#include "lim.h"
#include "cone.h"
#include "omni.h"

Data* data;
unsigned int sz;
char* rhs;
bool eq = false;

float* boundRng;
unsigned int r;

Attr attr;

void* attrGet() {
	return &attr;
}

extern "C" void* boundRngGet() {
	return boundRng;
}

std::vector<void*> vehicle;
extern "C" void** vehicleGet() {
	return &vehicle[0];
}

std::vector<Obj*> mesh;
std::vector<Obj*> pt;

extern "C" void* rhsGet() {
	return rhs;
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
		}
	}

	// data
	if (serial["data"]["state"].type() == nlohmann::json::value_t::array) {
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

				data = dataMk(init, sz, pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));
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

				data = dataMk(init, x, y, pair.key(), glm::vec3(0.0, 0.0, -(((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2)) * y)));
				sz = x * y;
			}
		}
	}

	for (const auto& entry : serial["rhs"]) {
		rhs = (char*) malloc(0);

		if (entry.type() == nlohmann::json::value_t::number_unsigned) {
			sz++;

			rhs = (char*) realloc(rhs, sz);
			rhs[sz - 1] = (char) ((int) entry);
		}

		if (entry.type() == nlohmann::json::value_t::array) {
			for (const auto& byte : entry) {
				sz++;

				rhs = (char*) realloc(rhs, sz);
				rhs[sz - 1] = (char) ((int) entry);
			}
		}
	}

	// object
	for (const auto& entry : serial["obj"]) {
		Obj* obj = objMk(entry["name"], "obj", "dir", true, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

		mesh.push_back(obj);
	}

	// bound
	boundRng = (float*) malloc(0);

	for (const auto& entry : serial["bound"].items()) {
		if (entry.key() == "rng") {
			for (const auto& rng : entry.value()) {
				for (const auto& bound : rng) {
					for (auto it = bound.begin(); it != bound.end(); ++it) {
						glm::vec3 loc = glm::vec3(0.0);
						glm::vec3 rot = glm::vec3(0.0);
						if (it.key() == "X") {
							r++;

							boundRng = (float*) realloc(boundRng, r * sizeof (float));
							boundRng[r - 1] = it.value();

							loc[X] = it.value();
						}

						if (it.key() == "Z") {
							r++;

							boundRng = (float*) realloc(boundRng, r * sizeof (float));
							boundRng[r - 1] = it.value();

							loc[Z] = it.value();
						}

						Lim* lim = limMk(loc, rot);

						mesh.push_back(lim->_parent);
					}
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
