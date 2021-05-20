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
#include "dict.h"
#include "street_light.h"
#include "road.h"

typedef struct {
	void* _ptr;
	size_t _sz;
} cArr;

Var** data;
Var** goal;
unsigned int* type;
unsigned int noData;
bool eq = false;

Var** dataGet() {
	return data;
}

Var** goalGet() {
	return goal;
}

unsigned int* typeGet() {
	return type;
}

unsigned int noDataGet() {
	return noData;
}

Lim** boundRng;
unsigned int noBoundRng;

Cone** boundArea;
unsigned int noBoundArea;

Lim** boundRngGet() {
	return boundRng;
}

unsigned int noBoundRngGet() {
	return noBoundRng;
}

Cone** boundAreaGet() {
	return boundArea;
}

unsigned int noBoundAreaGet() {
	return noBoundArea;
}

Crane** crane;
Truck** truck;
CargoShip** cargoShip;

unsigned int noCrane;
unsigned int noTruck;
unsigned int noCargoShip;

Crane** craneGet() {
	return crane;
}

Truck** truckGet() {
	return truck;
}

CargoShip** cargoShipGet() {
	return cargoShip;
}

unsigned int noCraneGet() {
	return noCrane;
}

unsigned int noTruckGet() {
	return noTruck;
}

unsigned int noCargoShipGet() {
	return noCargoShip;
}

std::vector<Obj*> mesh;
std::vector<Obj*> line;
std::vector<Obj*> pt;

StreetLight** streetLight;
StreetLight** streetLightGet() {
	return streetLight;
}

unsigned int noStreetLight;
unsigned int noStreetLightGet() {
	return noStreetLight;
}

void scn::init(unsigned int stage, unsigned int lvl) {
	nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + omni::stage[stage] + "/" + std::to_string(lvl) + ".json"));

	/* de-allocate */
	// data
	for (int i = 0; i < noData; i++) {
		free(data[i]);
		free(goal[i]);
	}
	free(data);
	free(goal);

	// vehicle
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

	for (void* _ : line) {
		free(_);
	}
	line.clear();

	for (void* _ : pt) {
		free(_);
	}
	pt.clear();

	// bound
	for (int i = 0; i < noBoundRng; i++) {
		free(boundRng[i]);
	}
	free(boundRng);

	for (int i = 0; i < noBoundArea; i++) {
		free(boundArea[i]);
	}
	free(boundArea);

	/* allocate */
	// vehicle
	crane = (Crane**) malloc(0);
	noCrane = 0;

	truck = (Truck**) malloc(0);
	noTruck = 0;

	cargoShip = (CargoShip**) malloc(0);
	noCargoShip = 0;

	for (const auto& entry : serial["vehicle"]) {
		if (entry["name"] == "crane") {
			glm::vec3 loc = glm::vec3(0.0);
			if (entry.contains("loc")) {
				loc = util::json::vec(entry["loc"]);
			}

			glm::vec3 rot = glm::vec3(0.0);
			if (entry.contains("rot")) {
				rot = util::json::vec(entry["rot"]);
			}

			Crane* _ = craneMk(loc, rot);

			noCrane++;
			crane = (Crane**) realloc(crane, noCrane * sizeof (Crane*));
			crane[noCrane - 1] = _;

			mesh.push_back(_->_parent);
		}

		if (entry["name"] == "cargo_ship") {
			char* init = (char*) malloc(entry["data"].size() * sizeof (char));
			unsigned int no = 0;
			for (const auto& byte : entry["data"]) {
				init[no] = (char) ((int) byte);

				no++;
			}

			glm::vec3 loc = glm::vec3(0.0);
			if (entry.contains("loc")) {
				loc = util::json::vec(entry["loc"]);
			}

			glm::vec3 rot = glm::vec3(0.0);
			if (entry.contains("rot")) {
				rot = util::json::vec(entry["rot"]);
			}

			CargoShip* _ = cargoShipMk(init, no, loc, rot);

			noCargoShip++;
			cargoShip = (CargoShip**) realloc(cargoShip, noCargoShip * sizeof (CargoShip*));
			cargoShip[noCargoShip - 1] = _;

			mesh.push_back(_->_parent);
		}

		if (entry["name"] == "truck") {
			char* init = (char*) malloc(entry["data"].size() * sizeof (char));
			unsigned int no = 0;
			for (const auto& _ : entry["data"]) {
				init[no] = (char) ((int) _);

				no++;
			}

			glm::vec3 loc = glm::vec3(0.0);
			if (entry.contains("loc")) {
				loc = util::json::vec(entry["loc"]);
			}

			glm::vec3 rot = glm::vec3(0.0);
			if (entry.contains("rot")) {
				rot = util::json::vec(entry["rot"]);
			}

			Truck* _ = truckMk(init, no, loc, rot);

			noTruck++;
			truck = (Truck**) realloc(truck, noTruck * sizeof (Truck*));
			truck[noTruck - 1] = _;

			mesh.push_back(_->_parent);
		}
	}

	// path
	for (const auto& strip : serial["road"]["path"]) {
		for (int i = 0; i < strip.size() - 1; i++) {
			if (strip[1].type() == nlohmann::json::value_t::number_unsigned) {
				GLushort pt[2] = {
					strip[i],
					strip[i + 1]
				};

				GLfloat vtc[2 * 3];
				for (int p = 0; p < 2; p++) {
					for (int a = 0; a < 3; a++) {
						vtc[(p * 3) + a] = serial["road"]["node"][pt[p]][a];
					}
				}

				line.push_back(lineMk(vtc, "obj", "thick", "solid"));
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
							vtc[(p * 3) + a] = serial["road"]["node"][pt[p]][a];
						}
					}

					line.push_back(lineMk(vtc, "obj", "thick", "solid"));
				}
			}
		}
	}

	/* data */
	data = (Var**) malloc(serial["data"].size() * sizeof (Var*));
	type = (unsigned int*) malloc(serial["data"].size() * sizeof (unsigned int*));

	for (const auto& pair : serial["data"].items()) {
		// scalar
		if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
			char init = (char) ((int) pair.value());

			Idx* val = idxMk(0, &init, 1, pair.key());

			for (const auto& pair : serial["data"].items()) {
				char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
				for (int i = 0; i < pair.key().size(); i++) {
					id[i] = pair.key()[i];
				}
				id[pair.key().size()] = '\0';

				Var* _ = varMk(id, val);

				data[noData] = _;
				type[noData] = SCALAR;
				noData++;

				mesh.push_back(val->_parent);
			}
		}

		// array
		if (pair.value().type() == nlohmann::json::value_t::array) {
			for (const auto& pair : serial["data"].items()) {
				const auto cont = pair.value();

				// 1D
				if (pair.value()[0].type() == nlohmann::json::value_t::number_unsigned) {
					unsigned int x = 0;
					char* init = (char*) malloc(x * sizeof (char));
					for (const auto& item : pair.value()) {
						if (item.type() == nlohmann::json::value_t::number_unsigned) {
							init[x] = (char) ((int) item);

							x++;
						}
					}

					Arr* val = arrMk(init, x, pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))));

					char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
					for (int i = 0; i < pair.key().size(); i++) {
						id[i] = pair.key()[i];
					}
					id[pair.key().size()] = '\0';

					Var* _ = varMk(id, val);

					data[noData] = _;
					type[noData] = ARRAY;
					noData++;

					mesh.push_back(((Arr*) (((Var*) data[noData - 1])->_ptr))->_parent);
				}

				// matrix
				if (pair.value()[0].type() == nlohmann::json::value_t::array) {
					// 2D
					if (pair.value()[0][0].type() == nlohmann::json::value_t::number_unsigned) {
						char* init = (char*) malloc(pair.value().size() * pair.value()[0].size() * sizeof (char));

						unsigned int x = 0;
						unsigned int y = 0;
						for (const auto& arr : pair.value()) {
							x = 0;

							y++;

							if (arr.type() == nlohmann::json::value_t::array) {
								for (const auto& byte : arr) {
									x++;
								}	
							}
						}

						unsigned int no = 0;
						for (const auto& arr : pair.value()) {
							for (const auto& byte : arr) {
								init[no] = (char) ((int) byte);

								no++;
							}	
						}

						init = (char*) realloc(init, x * y * sizeof (char));

						char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
						for (int i = 0; i < pair.key().size(); i++) {
							id[i] = pair.key()[i];
						}
						id[pair.key().size()] = '\0';

						Arr* val = arrMk(init, x, y, pair.key(), glm::vec3(0.0, 0.0, -((layout::idx[Z] / 2) + (layout::offset * 2) + (layout::margin * 2))));

						noData++;
						Var* _ = varMk(id, val);
						data[noData - 1] = _;

						mesh.push_back(((Arr*) (((Var*) data[noData - 1])->_ptr))->_parent);
					}

					// 3D
					if (pair.value()[0][0].type() == nlohmann::json::value_t::array) {
						char* init = (char*) malloc(cont.size() * cont[0].size() * cont[0][0].size() * sizeof (char));

						int no = 0;
						for (int k = 0; k < cont.size(); k++) {
							for (int j = 0; j < cont[k].size(); j++) {
								for (int i = 0; i < cont[k][j].size(); i++) {
									init[no] = (char) ((int) cont[k][j][i]);

									no++;
								}
							}
						}

						char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
						for (int i = 0; i < pair.key().size(); i++) {
							id[i] = pair.key()[i];
						}
						id[pair.key().size()] = '\0';

						Arr* val = arrMk(init, cont.size(), cont[0].size(), cont[0][0].size(), pair.key());

						Var* _ = varMk(id, val);

						noData++;
						data = (Var**) realloc(data, noData * sizeof (void*));
						type = (unsigned int*) realloc(type, noData * sizeof (unsigned int*));
						data[noData - 1] = _;
						type[noData - 1] = ARRAY;

						mesh.push_back(val->_parent);
					}
				}
			}
		}

		// dictionary
		if (pair.value().type() == nlohmann::json::value_t::object) {
			unsigned int no = pair.value().size();
			void** data = (void**) malloc(no * sizeof (void*));
			unsigned int* type = (unsigned int*) malloc(no * sizeof (unsigned int));

			for (const auto& pair : pair.value().items()) {
				void* _;
				unsigned int t;

				// scalar
				if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
					char* init = (char*) malloc(sizeof (char));
					init[0] = (char) ((int) pair.value());

					_ = idxMk(0, init, 1);
					t = SCALAR;
				}

				// array
				if (pair.value().type() == nlohmann::json::value_t::array) {
					// 1D
					if (pair.value()[0].type() == nlohmann::json::value_t::number_unsigned) {
						unsigned int no = pair.value();
						char* init = (char*) malloc(no * sizeof (char));
						for (int i = 0; i < no; i++) {
							init[i] = (char) ((int) pair.value()[i]);
						}

						_ = idxMk(0, init, no);
						t = ARRAY;
					}
				}

				data[no] = _;
				type[no] = t;
				no++;
			}	

			Dict* _ = dictMk(data, type, no);

			mesh.push_back(_->_parent);
		}
	}

	/* goal */
	goal = (Var**) malloc(serial["goal"].size() * sizeof (Var*));
	unsigned int g = 0;

	for (const auto& pair : serial["goal"].items()) {
		char* id = (char*) malloc((pair.key().size() + 1) * sizeof (char));
		for (int i = 0; i < pair.key().size(); i++) {
			id[i] = pair.key().size();
		}
		id[pair.key().size()] = '\0';

		void* val;

		// scalar
		if (pair.value().type() == nlohmann::json::value_t::number_unsigned) {
			char init = (char) ((int) (pair.value()));

			val = idxMk(0, &init, 1, pair.key());
		}

		// array
		if (pair.value().type() == nlohmann::json::value_t::array) {
			const auto cont = pair.value();

			// 1D
			if (cont[0].type() == nlohmann::json::value_t::number_unsigned) {
				char* init = (char*) malloc(cont.size() * sizeof (char));
				for (int i = 0; i < cont.size(); i++) {
					init[i] = (char) ((int) cont[i]);
				}

				val = arrMk(init, cont.size(), pair.key());
			}

			if (cont[0].type() == nlohmann::json::value_t::array) {
				// 2D
				if (cont[0][0].type() == nlohmann::json::value_t::number_unsigned) {
					char* init = (char*) malloc(cont.size() * cont[0].size() * sizeof (char));

					for (int j = 0; j < cont.size(); j++) {
						for (int i = 0; i < cont[j].size(); i++) {
							init[(j * cont[0].size()) + i] = (char) ((int) cont[j][i]);
						}
					}

					val = arrMk(init, cont.size() * cont[0].size(), pair.key());
				}

				// 3D
				if (cont[0][0].type() == nlohmann::json::value_t::array) {
					char* init = (char*) malloc(cont.size() * cont[0].size() * cont[0][0].size() * sizeof (char));

					for (int k = 0; k < cont.size(); k++) {
						for (int j = 0; j < cont[k].size(); j++) {
							for (int i = 0; i < cont[k][j].size(); i++) {
								init[(k * j * cont[0].size()) + i] = (char) ((int) cont[j][i]);
							}
						}
					}

					val = arrMk(init, cont.size() * cont[0].size() * cont[0][0].size(), pair.key());
				}
			}
		}
		Var* _ = varMk(id, val);

		goal[g] = _;
		g++;
	}

	// prop
	for (const auto& entry : serial["prop"]) {
		glm::vec3 loc = glm::vec3(0.0);
		if (entry.contains("loc")) {
			loc = util::json::vec(entry["loc"]);
		}

		glm::vec3 rot = glm::vec3(0.0);
		if (entry.contains("rot")) {
			rot = util::json::vec(entry["rot"]);
		}

		Obj* obj = objMk(entry["name"], "obj", "dir", true, loc, rot);

		mesh.push_back(obj);
	}

	// bound
	boundRng = (Lim**) malloc(serial["bound"]["rng"].size() * sizeof (Lim*));
	noBoundRng = 0;

	boundArea = (Cone**) malloc(serial["bound"]["area"].size() * sizeof (Cone*));
	noBoundArea = 0;

	for (const auto& entry : serial["bound"].items()) {
		if (entry.key() == "rng") {
			for (const auto& rng : entry.value()) {
				for (const auto& lim : rng) {
					for (auto it = lim.begin(); it != lim.end(); ++it) {
						unsigned int axis;
						if (it.key() == "X") {
							axis = X;
						}

						if (it.key() == "Z") {
							axis = Z;
						}

						Lim* _ = limMk(axis, it.value());

						boundRng[noBoundRng] = _;
						noBoundRng++;

						line.push_back(_->_parent);
					}
				}
			}
		}

		if (entry.key() == "area") {
			for (const auto& entry : entry.value()) {
				GLfloat init[2][2];
				for (int y = 0; y < 2; y++) {
					for (int x = 0; x < 2; x++) {
						init[y][x] = entry["init"][y][x];
					}
				}

				glm::vec3 loc = glm::vec3(0.0);
				if (entry.contains("loc")) {
					loc = util::json::vec(entry["loc"]);
				}

				Cone* _ = coneMk(init, loc);

				for (int i = 0; i < 3; i++) {
					boundArea[((noBoundArea - 1) * 3) + i] = _;
				}
				noBoundArea++;

				mesh.push_back(_->_parent);
			}
		}
	}

	// control flow
	streetLight = (StreetLight**) malloc(serial["ctrl"].size() * sizeof (StreetLight*));
	noStreetLight = 0;

	for (const auto& entry : serial["ctrl"].items()) {
		unsigned int no = entry.value()["pass"].size();
		bool* pass = (bool*) malloc(no * sizeof (bool));
		for (int i = 0; i < no; i++) {
			pass[i] = entry.value()["pass"][i];
		}

		StreetLight* _ = streetLightMk(pass, no);

		streetLight[noStreetLight] = _;
		noStreetLight++;

		mesh.push_back(_->_parent);
	}
}
