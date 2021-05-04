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
#include "cargo_ship.h"
#include "omni.h"

void** data;
unsigned int s;
char* goal;
bool eq = false;

unsigned int szGet() {
	return s;
}

Lim* boundRng;
unsigned int r;

void** coneRng;
unsigned int c = 0;

extern "C" void* boundRngGet() {
	return boundRng;
}

extern "C" void** coneRngGet() {
	return coneRng;
}

std::vector<void*> vehicle;
extern "C" void** vehicleGet() {
	return &vehicle[0];
}

std::vector<Obj*> mesh;
std::vector<Obj*> pt;

extern "C" void** dataGet() {
	return data;
}

extern "C" void* goalGet() {
	return goal;
}

void scn::init(unsigned int stage, unsigned int lvl) {
	nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + omni::stage[stage] + "/" + std::to_string(lvl) + ".json"));

	for (void* _ : vehicle) {
		free(_);
	}
	vehicle.clear();

	for (void* _ : mesh) {
		free(_);
	}
	mesh.clear();

	// vehicle
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

	/* data */
	data = (void**) malloc(0);

	// scalar
	if (serial["data"]["state"].type() == nlohmann::json::value_t::number_unsigned) {
		Idx* val = idxMk(0, (char) ((int) serial["data"]["state"]));

		for (const auto& map : serial["data"].items()) {
			char* id = (char*) malloc(0);
			id = (char*) realloc(id, (map.key().size() + 1) * sizeof (char));
			for (int i = 0; i < map.key().size(); i++) {
				id[i] = map.key()[i];
			}
			id[map.key().size()] = '\0';

			s++;
			Var* var = varMk(id, val);
			data[s - 1] = var;

			mesh.push_back(val->_parent);
		}
	}

	// array
	if (serial["data"]["state"].type() == nlohmann::json::value_t::array) {
		for (const auto& map : serial["data"].items()) {
			// 1D
			if (serial["data"]["state"][0].type() == nlohmann::json::value_t::number_unsigned) {
				char* init = (char*) malloc(0);

				unsigned int x = 0;
				for (const auto& item : serial["data"][map.key()]) {
					if (item.type() == nlohmann::json::value_t::number_unsigned) {
						x++;
						init = (char*) realloc(init, x * sizeof (char));
						init[x - 1] = (char) ((int) item);
					}
				}

				Arr* val = arrMk(init, x, map.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));

				char* id = (char*) malloc(0);
				id = (char*) realloc(id, (map.key().size() + 1) * sizeof (char));
				for (int i = 0; i < map.key().size(); i++) {
					id[i] = map.key()[i];
				}
				id[map.key().size()] = '\0';

				s++;
				Var* var = varMk(id, val);
				data[s - 1] = var;

				mesh.push_back(((Arr*) (((Var*) data[s - 1])->_ptr))->_parent);
			}

			if (serial["data"]["state"][0].type() == nlohmann::json::value_t::array) {
				// 2D
				if (serial["data"]["state"][0][0].type() == nlohmann::json::value_t::number_unsigned) {
					char* init = (char*) malloc(0);

					unsigned int x = 0;
					unsigned int y = 0;
					for (const auto& arr : serial["data"][map.key()]) {
						x = 0;

						y++;

						if (arr.type() == nlohmann::json::value_t::array) {
							for (const auto& scal : arr) {
								x++;
							}	
						}
					}

					init = (char*) realloc(init, x * y * sizeof (char));

					unsigned int sz = 0;
					for (const auto& arr : serial["data"][map.key()]) {
						for (const auto& scal : arr) {
							init[sz] = (char) ((int) scal);

							sz++;
						}	
					}

					init = (char*) realloc(init, x * y * sizeof (char));

					Arr* val = arrMk(init, x, y, map.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));

					char* id = (char*) malloc(0);
					id = (char*) realloc(id, (map.key().size() + 1) * sizeof (char));
					for (int i = 0; i < map.key().size(); i++) {
						id[i] = map.key()[i];
					}
					id[map.key().size()] = '\0';

					s++;
					Var* var = varMk(id, val);
					data[s - 1] = var;

					mesh.push_back(((Arr*) (((Var*) data[s - 1])->_ptr))->_parent);
				}

				// 3D
				if (serial["data"]["state"][0][0].type() == nlohmann::json::value_t::array) {
					char* init = (char*) malloc(0);
					unsigned int bound[3] = {
						0, 0, 0
					};

					for (const auto& byte : serial["data"][map.key()][0][0]) {
						bound[X]++;
					}

					for (const auto& arr : serial["data"][map.key()][0]) {
						bound[Y]++;
					}

					for (const auto& matr : serial["data"][map.key()]) {
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
				}
			}
		}
	}

	// goal
	goal = (char*) malloc(0);

	unsigned int i = 0;
	for (const auto& data : serial["goal"]) {
		if (data.type() == nlohmann::json::value_t::number_unsigned) {
			goal[i] = (char) ((int) data);

			i++;
		}

		if (data.type() == nlohmann::json::value_t::array) {
			for (const auto& byte : data) {
				goal[i] = (char) ((int) data);

				i++;
			}
		}
	}

	// object
	for (const auto& entry : serial["obj"]) {
		Obj* obj = objMk(entry["name"], "obj", "dir", true, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

		mesh.push_back(obj);
	}

	// bound
	boundRng = (Lim*) malloc(0);
	coneRng = (void**) malloc(0);

	for (const auto& entry : serial["bound"].items()) {
		if (entry.key() == "rng") {
			for (const auto& rng : entry.value()) {
				for (const auto& bound : rng) {
					for (auto it = bound.begin(); it != bound.end(); ++it) {
						unsigned int axis;
						glm::vec3 loc = glm::vec3(0.0);
						glm::vec3 rot = glm::vec3(M_PI / 2, 0.0, 0.0);
						if (it.key() == "X") {
							r++;

							boundRng = (Lim*) realloc(boundRng, r * sizeof (Lim));

							boundRng[r - 1] = {
								X,
								it.value()
							};

							axis = 0;
							loc[X] = it.value();
							rot[Y] = 0.0;
						}

						if (it.key() == "Z") {
							r++;

							boundRng = (Lim*) realloc(boundRng, r * sizeof (Lim));

							boundRng[r - 1] = {
								Z,
								it.value()
							};

							axis = 2;
							loc[Z] = it.value();
							rot[Y] = M_PI / 2;
						}

						Lim* lim = limMk(axis, it.value(), loc, rot);

						mesh.push_back(lim->_parent);
					}
				}
			}
		}

		if (entry.key() == "cone") {
			for (const auto& entry : entry.value()) {
				GLfloat bound[2][2];
				for (int y = 0; y < 2; y++) {
					for (int x = 0; x < 2; x++) {
						bound[x][y] = entry["bound"][x][y];
					}
				}

				Cone* cone = coneMk(bound, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]));

				c++;

				coneRng = (void**) realloc(coneRng, c * 3 * sizeof (float));
				for (int i = 0; i < 3; i++) {
					coneRng[((c - 1) * 3) + i] = cone;
				}

				mesh.push_back(cone->_parent);
			}
		}
	}
}
