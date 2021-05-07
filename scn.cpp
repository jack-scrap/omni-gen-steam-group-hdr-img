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
#include "line.h"

void** data;
unsigned int noData;
void** goal;
bool eq = false;

extern "C" void** dataGet() {
	return data;
}

unsigned int noDataGet() {
	return noData;
}

extern "C" void** goalGet() {
	return goal;
}

Lim* boundRng;
unsigned int r;

void** areaRng;
unsigned int a = 0;

extern "C" void* boundRngGet() {
	return boundRng;
}

extern "C" void** areaRngGet() {
	return areaRng;
}

Crane** crane;
Truck** truck;
CargoShip** cargoShip;

unsigned int noCrane;
unsigned int noTruck;
unsigned int noCargoShip;

extern "C" Crane** craneGet() {
	return crane;
}

extern "C" Truck** truckGet() {
	return truck;
}

extern "C" CargoShip** cargoShipGet() {
	return cargoShip;
}

extern "C" unsigned int noCraneGet() {
	return noCrane;
}

extern "C" unsigned int noTruckGet() {
	return noTruck;
}

extern "C" unsigned int noCargoShipGet() {
	return noCargoShip;
}

std::vector<Obj*> mesh;
std::vector<Obj*> line;
std::vector<Obj*> pt;

void scn::init(unsigned int stage, unsigned int lvl) {
	nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + omni::stage[stage] + "/" + std::to_string(lvl) + ".json"));


	for (int i = 0; i < noCrane; i++) {
		free(crane[i]);
	}
	free(crane);

	for (int i = 0; i < noTruck; i++) {
		free(truck[i]);
	}
	free(truck);

	for (int i = 0; i < noCargoShip; i++) {
		free(cargoShip[i]);
	}
	free(cargoShip);

	for (void* _ : mesh) {
		free(_);
	}
	mesh.clear();

	// vehicle
	crane = (Crane**) malloc(0);
	noCrane = 0;

	truck = (Truck**) malloc(0);
	noTruck = 0;

	cargoShip = (CargoShip**) malloc(0);
	noCargoShip = 0;

	for (const auto& entry : serial["vehicle"]) {
		if (entry["name"] == "crane") {
			Crane* _ = craneMk(glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			noCrane++;
			crane = (Crane**) realloc(crane, noCrane * sizeof (Crane*));
			crane[noCrane - 1] = _;

			mesh.push_back(_->_parent);
		}

		if (entry["name"] == "cargo_ship") {
			char* init = (char*) malloc(entry["data"].size() * sizeof (char));
			unsigned int i = 0;
			for (const auto& byte : entry["data"]) {
				init[i] = (char) ((int) byte);

				i++;
			}

			CargoShip* _ = cargoShipMk(init, i, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			noCargoShip++;
			cargoShip = (CargoShip**) realloc(cargoShip, noCargoShip * sizeof (CargoShip*));
			cargoShip[noCargoShip - 1] = _;

			mesh.push_back(_->_parent);
		}

		if (entry["name"] == "truck") {
			char* init = (char*) malloc(entry["data"].size() * sizeof (char));
			unsigned int i = 0;
			for (const auto& _ : entry["data"]) {
				init[i] = (char) ((int) _);

				i++;
			}

			Truck* _ = truckMk(init, i, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]), glm::vec3(entry["rot"][X], entry["rot"][Y], entry["rot"][Z]));

			noTruck++;
			truck = (Truck**) realloc(truck, noTruck * sizeof (Truck*));
			truck[noTruck - 1] = _;

			mesh.push_back(_->_parent);
		}
	}

	// path
	for (const auto& strip : serial["path"]["link"]) {
		for (int i = 0; i < strip.size() - 1; i++) {
			if (strip[1].type() == nlohmann::json::value_t::number_unsigned) {
				GLushort pt[2] = {
					strip[i],
					strip[i + 1]
				};

				GLfloat vtc[2 * 3];
				for (int p = 0; p < 2; p++) {
					for (int a = 0; a < 3; a++) {
						vtc[(p * 3) + a] = serial["path"]["node"][pt[p]][a];
					}
				}

				line.push_back(lineMk(vtc));
			}

			if (strip[1].type() == nlohmann::json::value_t::array) {
				for (const auto& no : strip[1]) {
					GLushort pt[2] = {
						strip[i],
						no
					};

					GLfloat vtc[2 * 3];
					for (int p = 0; p < 2; p++) {
						for (int a = 0; a < 3; a++) {
							vtc[(p * 3) + a] = serial["path"]["node"][pt[p]][a];
						}
					}

					line.push_back(lineMk(vtc));
				}
			}
		}
	}

	/* data */
	data = (void**) malloc(0);

	// scalar
	for (const auto& pair : serial["data"].items()) {
		if (serial["data"][pair.key()].type() == nlohmann::json::value_t::number_unsigned) {
			Idx* val = idxMk(0, (char) ((int) serial["data"][pair.key()]));

			for (const auto& pair : serial["data"].items()) {
				char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
				for (int i = 0; i < pair.key().size(); i++) {
					id[i] = pair.key()[i];
				}
				id[pair.key().size()] = '\0';

				noData++;
				Var* var = varMk(id, val);
				data[noData - 1] = var;

				mesh.push_back(val->_parent);
			}
		}

		// array
		if (serial["data"][pair.key()].type() == nlohmann::json::value_t::array) {
			for (const auto& pair : serial["data"].items()) {
				// 1D
				if (serial["data"][pair.key()][0].type() == nlohmann::json::value_t::number_unsigned) {
					unsigned int x = 0;
					char* init = (char*) malloc(x * sizeof (char));
					for (const auto& item : serial["data"][pair.key()]) {
						if (item.type() == nlohmann::json::value_t::number_unsigned) {
							init[x] = (char) ((int) item);

							x++;
						}
					}

					Arr* val = arrMk(init, x, pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));

					char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
					for (int i = 0; i < pair.key().size(); i++) {
						id[i] = pair.key()[i];
					}
					id[pair.key().size()] = '\0';

					noData++;
					Var* var = varMk(id, val);
					data[noData - 1] = var;

					mesh.push_back(((Arr*) (((Var*) data[noData - 1])->_ptr))->_parent);
				}

				if (serial["data"][pair.key()][0].type() == nlohmann::json::value_t::array) {
					// 2D
					if (serial["data"][pair.key()][0][0].type() == nlohmann::json::value_t::number_unsigned) {
						char* init = (char*) malloc(serial["data"][pair.key()].size() * serial["data"][pair.key()][0].size() * sizeof (char));

						unsigned int x = 0;
						unsigned int y = 0;
						for (const auto& arr : serial["data"][pair.key()]) {
							x = 0;

							y++;

							if (arr.type() == nlohmann::json::value_t::array) {
								for (const auto& scal : arr) {
									x++;
								}	
							}
						}

						unsigned int no = 0;
						for (const auto& arr : serial["data"][pair.key()]) {
							for (const auto& scal : arr) {
								init[no] = (char) ((int) scal);

								no++;
							}	
						}

						init = (char*) realloc(init, x * y * sizeof (char));

						Arr* val = arrMk(init, x, y, pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Y] / 2) + (layout::offset * 2) + (layout::margin * 2))));

						char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
						for (int i = 0; i < pair.key().size(); i++) {
							id[i] = pair.key()[i];
						}
						id[pair.key().size()] = '\0';

						noData++;
						Var* var = varMk(id, val);
						data[noData - 1] = var;

						mesh.push_back(((Arr*) (((Var*) data[noData - 1])->_ptr))->_parent);
					}

					// 3D
					if (serial["data"][pair.key()][0][0].type() == nlohmann::json::value_t::array) {
						char* init = (char*) malloc(2 * 2 * 2 * sizeof (char));
						unsigned int bound[3] = {
							0, 0, 0
						};

						for (const auto& byte : serial["data"][pair.key()][0][0]) {
							bound[X]++;
						}

						for (const auto& arr : serial["data"][pair.key()][0]) {
							bound[Y]++;
						}

						for (const auto& matr : serial["data"][pair.key()]) {
							bound[Z]++;
						}

						unsigned int no = 0;
						for (int z = 0; z < 2; z++) {
							for (int y = 0; y < 2; y++) {
								for (int x = 0; x < 2; x++) {
									init[no] = 'a';

									no++;
								}
							}
						}
					}
				}
			}
		}

		// dictionary
		if (serial["data"][pair.key()].type() == nlohmann::json::value_t::object) {
			Var** init = (Var**) malloc(0);
			unsigned int no = 0;

			for (const auto& pair : serial["data"][pair.key()].items()) {
				char* id = (char*) malloc(pair.key().size() * sizeof (char));
				for (int i = 0; i < pair.key().size(); i++) {
					id[i] = pair.key()[i];
				}

				char* val = (char*) malloc(0);
				unsigned int v = 0;

				// scalar
				if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
					v++;
					val = (char*) realloc(val, v * sizeof (char));
					val[v - 1] = (char) ((int) pair.value());
				}

				// array
				if (pair.value().type() == nlohmann::json::value_t::array) {
					for (int i = 0; i < pair.value().size(); i++) {
						v++;
						val = (char*) realloc(val, v * sizeof (char));
						val[v - 1] = (char) ((int) pair.value()[i]);
					}
				}

				no++;
				init = (Var**) realloc(init, no * sizeof (long long));
				init[0] = varMk(id, 0);
			}
		}
	}

	/* goal */
	unsigned int g = 0;
	for (const auto& pair : serial["goal"].items()) {
		// scalar
		if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
			g++;
			goal = (void**) malloc(g * sizeof (void*));
			goal[g - 1] = idxMk(0, (char) ((int) pair.value()), pair.key());
		}

		// array
		if (pair.value().type() == nlohmann::json::value_t::array) {
			const auto cont = pair.value();

			// 1D
			if (cont[0].type() == nlohmann::json::value_t::number_unsigned) {
				char* init = (char*) malloc(cont.size() * sizeof (char*));
				for (int i = 0; i < cont.size(); i++) {
					init[i] = (char) ((int) cont[i]);
				}

				g++;
				goal = (void**) realloc(goal, g * sizeof (void*));
				goal[g - 1] = arrMk(init, cont.size(), pair.key());
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
	areaRng = (void**) malloc(0);

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
						bound[y][x] = entry["bound"][y][x];
					}
				}

				Cone* _ = coneMk(bound, glm::vec3(entry["loc"][X], entry["loc"][Y], entry["loc"][Z]));

				a++;
				areaRng = (void**) realloc(areaRng, a * 3 * sizeof (float));
				for (int i = 0; i < 3; i++) {
					areaRng[((a - 1) * 3) + i] = _;
				}

				mesh.push_back(_->_parent);
			}
		}
	}
}
