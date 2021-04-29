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
#include "cargo_ship.h"
#include "omni.h"

Data* data;
unsigned int sz;
char* rhs;
bool eq = false;

End* boundRng;
unsigned int r;

float* coneRng;
unsigned int c = 0;

extern "C" void* boundRngGet() {
	return boundRng;
}

extern "C" void* coneRngGet() {
	return coneRng;
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

extern "C" void* dataGet() {
	return data;
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

		if (entry["name"] == "cargo_ship") {
			char* init = (char*) malloc(0);
			unsigned int sz = 0;

			for (const auto& byte : entry["data"]) {
				sz++;

				init = (char*) realloc(init, sz * sizeof (char));
				init[sz - 1] = (char) ((int) byte);
			}

			CargoShip* cargoShip = cargoShipMk(init, sz, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			vehicle.push_back(cargoShip);
			mesh.push_back(cargoShip->_parent);
		}

		if (entry["name"] == "truck") {
			char* init = (char*) malloc(0);
			unsigned int sz = 0;

			for (const auto& _ : entry["data"]) {
				sz++;

				init = (char*) realloc(init, sz * sizeof (char));
				init[sz - 1] = (char) ((int) _);
			}

			Truck* truck = truckMk(init, sz, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			vehicle.push_back(truck);
			mesh.push_back(truck->_parent);
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
			}

			if (serial["data"]["state"][0].type() == nlohmann::json::value_t::array) {
				// 2D
				if (serial["data"]["state"][0][0].type() == nlohmann::json::value_t::number_unsigned) {
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

				// 3D
				if (serial["data"]["state"][0][0].type() == nlohmann::json::value_t::array) {
					char* init = (char*) malloc(0);
					unsigned int bound[3] = {
						0, 0, 0
					};

					for (const auto& item : serial["data"][pair.key()][0][0]) {
						bound[X]++;
					}

					for (const auto& item : serial["data"][pair.key()][0]) {
						bound[Y]++;
					}

					for (const auto& item : serial["data"][pair.key()]) {
						bound[Z]++;
					}

					unsigned int sz = 0;
					for (int z = 0; z < 2; z++) {
						for (int y = 0; y < 2; y++) {
							for (int x = 0; x < 2; x++) {
								sz++;

								init = (char*) realloc(init, sz * sizeof (char));
								init[sz - 1] = 'a';
							}
						}
					}

					data = dataMk(init, bound[X], bound[Y], bound[Z], pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));
				}
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
	boundRng = (End*) malloc(0);
	coneRng = (float*) malloc(0);

	for (const auto& entry : serial["bound"].items()) {
		if (entry.key() == "rng") {
			for (const auto& rng : entry.value()) {
				for (const auto& bound : rng) {
					for (auto it = bound.begin(); it != bound.end(); ++it) {
						glm::vec3 loc = glm::vec3(0.0);
						glm::vec3 rot = glm::vec3(M_PI / 2, 0.0, 0.0);
						if (it.key() == "X") {
							r++;

							boundRng = (End*) realloc(boundRng, r * sizeof (End));

							boundRng[r - 1] = {
								X,
								it.value()
							};

							loc[X] = it.value();
							rot[Y] = 0.0;
						}

						if (it.key() == "Z") {
							r++;

							boundRng = (End*) realloc(boundRng, r * sizeof (End));

							boundRng[r - 1] = {
								Z,
								it.value()
							};

							loc[Z] = it.value();
							rot[Y] = M_PI / 2;
						}

						Lim* lim = limMk(loc, rot);

						mesh.push_back(lim->_parent);
					}
				}
			}
		}

		if (entry.key() == "cone") {
			for (const auto& entry : entry.value()) {
				c++;

				coneRng = (float*) realloc(coneRng, c * 3 * sizeof (float));
				for (int i = 0; i < 3; i++) {
					coneRng[(c * 3) - 3 + i] = entry[i];
				}

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
