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
			cArr init = util::json::arr(entry["data"]);

			glm::vec3 loc = glm::vec3(0.0);
			if (entry.contains("loc")) {
				loc = util::json::vec(entry["loc"]);
			}

			glm::vec3 rot = glm::vec3(0.0);
			if (entry.contains("rot")) {
				rot = util::json::vec(entry["rot"]);
			}

			CargoShip* _ = cargoShipMk((char*) init._ptr, init._x * init._y, loc, rot);

			noCargoShip++;
			cargoShip = (CargoShip**) realloc(cargoShip, noCargoShip * sizeof (CargoShip*));
			cargoShip[noCargoShip - 1] = _;

			mesh.push_back(_->_parent);
		}

		if (entry["name"] == "truck") {
			cArr init = util::json::arr(entry["data"]);

			glm::vec3 loc = glm::vec3(0.0);
			if (entry.contains("loc")) {
				loc = util::json::vec(entry["loc"]);
			}

			glm::vec3 rot = glm::vec3(0.0);
			if (entry.contains("rot")) {
				rot = util::json::vec(entry["rot"]);
			}

			Truck* _ = truckMk((char*) init._ptr, init._x, loc, rot);

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

				GLfloat vtc[2][3];
				for (int p = 0; p < 2; p++) {
					for (int a = 0; a < 3; a++) {
						vtc[p][a] = serial["road"]["node"][pt[p]][a];
					}
				}

				line.push_back(lineMk((GLfloat*) vtc, "obj", "thick", "solid"));
			}

			if (strip[1].type() == nlohmann::json::value_t::array) {
				for (const auto& no : strip[1]) {
					GLushort pt[2] = {
						strip[i],
						no
					};

					GLfloat vtc[2][3];
					for (int p = 0; p < 2; p++) {
						for (int a = 0; a < 3; a++) {
							vtc[p][a] = serial["road"]["node"][pt[p]][a];
						}
					}

					line.push_back(lineMk((GLfloat*) vtc, "obj", "thick", "solid"));
				}
			}
		}
	}

	/* data */
	// initial
	util::json::scope(serial, mesh);

	// desired
	util::json::scope(serial, mesh);

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
					for (const auto& pair : lim.items()) {
						unsigned int axis;
						if (pair.key() == "X") {
							axis = X;
						}

						if (pair.key() == "Z") {
							axis = Z;
						}

						Lim* _ = limMk(axis, pair.value());

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
