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

std::vector<Obj*> obj;
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
	nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + omni::stage[stage] + "/" + std::to_string(lvl) + ".json"));

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

	for (void* _ : obj) {
		free(_);
	}
	obj.clear();

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

	for (const auto& entry : deser["vehicle"]) {
		glm::vec3 loc = glm::vec3(0.0);
		if (entry.contains("loc")) {
			loc = util::json::vec(entry["loc"]);
		}

		glm::vec3 rot = glm::vec3(0.0);
		if (entry.contains("rot")) {
			rot = util::json::vec(entry["rot"]);
		}

		if (entry["name"] == "crane") {
			Crane* _ = craneMk(loc, rot);

			noCrane++;
			crane = (Crane**) realloc(crane, noCrane * sizeof (Crane*));
			crane[noCrane - 1] = _;

			obj.push_back(_->_parent);
		}

		if (entry["name"] == "cargo_ship") {
			cBuff init = util::json::arr(entry["data"]);

			CargoShip* _ = cargoShipMk((char*) init._ptr, init._x * init._y, loc, rot);

			noCargoShip++;
			cargoShip = (CargoShip**) realloc(cargoShip, noCargoShip * sizeof (CargoShip*));
			cargoShip[noCargoShip - 1] = _;

			obj.push_back(_->_parent);
		}

		if (entry["name"] == "truck") {
			cBuff init = util::json::arr(entry["data"]);

			Truck* _ = truckMk((char*) init._ptr, init._x, loc, rot);

			noTruck++;
			truck = (Truck**) realloc(truck, noTruck * sizeof (Truck*));
			truck[noTruck - 1] = _;

			obj.push_back(_->_parent);
		}
	}

	// path
	for (const auto& strip : deser["road"]["path"]) {
		for (int i = 0; i < strip.size() - 1; i++) {
			switch (strip[1].type()) {
				case nlohmann::json::value_t::number_unsigned: {
					GLushort pt[2] = {
						strip[i],
						strip[i + 1]
					};

					GLfloat vtc[2][3];
					for (int p = 0; p < 2; p++) {
						for (int a = 0; a < 3; a++) {
							vtc[p][a] = deser["road"]["node"][pt[p]][a];
						}
					}

					line.push_back(lineMk((GLfloat*) vtc, "obj", "thick", "solid"));

					break;
				}

				case nlohmann::json::value_t::array: {
					for (const auto& no : strip[1]) {
						GLushort pt[2] = {
							strip[i],
							no
						};

						GLfloat vtc[2][3];
						for (int p = 0; p < 2; p++) {
							for (int a = 0; a < 3; a++) {
								vtc[p][a] = deser["road"]["node"][pt[p]][a];
							}
						}

						line.push_back(lineMk((GLfloat*) vtc, "obj", "thick", "solid"));
					}

					break;
				}
			}
		}
	}

	/* data */
	noData = deser["data"].size();

	// initial
	util::json::scope(deser["data"], data, type, obj);

	// desired
	util::json::scope(deser["goal"], goal, type, obj);

	// prop
	for (const auto& entry : deser["prop"]) {
		util::json::prop(entry, obj);
	}

	// bound
	boundRng = (Lim**) malloc(deser["bound"]["rng"].size() * sizeof (Lim*));
	noBoundRng = 0;

	boundArea = (Cone**) malloc(deser["bound"]["area"].size() * sizeof (Cone*));
	noBoundArea = 0;

	for (const auto& entry : deser["bound"].items()) {
		if (entry.key() == "rng") {
			util::json::bound::rng(entry.value());
		}

		if (entry.key() == "area") {
			for (const auto& entry : entry.value()) {
				Cone* _ = util::json::bound::area(entry);
			}
		}
	}

	// control flow
	streetLight = (StreetLight**) malloc(deser["ctrl"].size() * sizeof (StreetLight*));
	noStreetLight = 0;

	for (const auto& entry : deser["ctrl"].items()) {
		StreetLight* _ = util::json::streetLight(entry.value());

		streetLight[noStreetLight] = _;
		noStreetLight++;
	}
}
