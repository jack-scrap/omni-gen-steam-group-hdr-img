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

CArr crane;
CArr truck;
CArr cargoShip;

CArr craneGet() {
	return crane;
}

CArr truckGet() {
	return truck;
}

CArr cargoShipGet() {
	return cargoShip;
}

std::vector<Obj*> scn::obj;
std::vector<Obj*> scn::line;
std::vector<Obj*> scn::pt;

CArr streetLight;
CArr streetLightGet() {
	return streetLight;
}

CArr boundRng;

CArr boundRngGet() {
	return boundRng;
}

CArr boundArea;

CArr boundAreaGet() {
	return boundArea;
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
	for (int i = 0; i < crane._sz; i += sizeof (Crane*)) {
		free(((Crane**) crane._ptr)[i]);
	}
	free(crane._ptr);

	for (int i = 0; i < truck._sz; i += sizeof (Truck*)) {
		free(((Truck**) truck._ptr)[i]);
	}
	free(truck._ptr);

	for (int i = 0; i < cargoShip._sz; i += sizeof (CargoShip*)) {
		free(((CargoShip**) cargoShip._ptr)[i]);
	}
	free(cargoShip._ptr);

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
	for (int i = 0; i < boundRng._sz; i += sizeof (Lim*)) {
		free(((Lim**) boundRng._ptr)[i]);
	}
	free(boundRng._ptr);

	for (int i = 0; i < boundArea._sz; i += sizeof (Cone*)) {
		free(((Cone**) boundArea._ptr)[i]);
	}
	free(boundArea._ptr);

	/* allocate */
	// vehicle
	crane._ptr = (Crane**) malloc(0);
	crane._sz = 0;

	truck._ptr = (Truck**) malloc(0);
	truck._sz = 0;

	cargoShip._ptr = (CargoShip**) malloc(0);
	cargoShip._sz = 0;

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

			crane._sz += sizeof (Crane*);
			crane._ptr = (Crane**) realloc(crane._ptr, crane._sz);
			((Crane**) crane._ptr)[crane._sz - sizeof (Crane*)] = _;

			obj.push_back(_->_parent);
		}

		if (entry["name"] == "cargo_ship") {
			CBuff init = util::json::arr::arr(entry["data"]);

			CargoShip* _ = cargoShipMk((char*) init._ptr, loc, rot);

			cargoShip._sz += sizeof (CargoShip*);
			cargoShip._ptr = (CargoShip**) realloc(cargoShip._ptr, cargoShip._sz);
			((CargoShip**) cargoShip._ptr)[cargoShip._sz - sizeof (CargoShip*)] = _;

			obj.push_back(_->_parent);
		}

		if (entry["name"] == "truck") {
			CBuff init = util::json::arr::arr(entry["data"]);

			Truck* _ = truckMk((char*) init._ptr, init._x, loc, rot);

			truck._sz += sizeof (Truck*);
			truck._ptr = (Truck**) realloc(truck._ptr, truck._sz);
			((Truck**) truck._ptr)[truck._sz - sizeof (Truck*)] = _;

			obj.push_back(_->_parent);

			pt.push_back(_->_parent->_child[((_->_no * 2) + 1)]);
			pt.push_back(_->_parent->_child[((_->_no * 2) + 1 + 1)]);
		}
	}

	// path
	for (const auto& strip : deser["road"]["path"]) {
		std::vector<Obj*> _ = util::json::path(strip, deser["road"]["node"]);

		for (Obj* seg : _) {
			line.push_back(seg);
		}
	}

	/* data */
	noData = deser["data"].size();

	type = (unsigned int*) malloc(noData * sizeof (unsigned int*));

	// initial
	data = util::json::scope(deser["data"], type);

	// desired
	goal = util::json::scope(deser["goal"], type);

	for (int i = 0; i < noData; i++) {
		Obj* _;

		switch (type[i]) {
			case omni::SCALAR:
				_ = ((Idx*) data[i]->_ptr)->_parent;

				break;

			case omni::ARRAY:
				_ = ((Arr*) data[i]->_ptr)->_parent;

				break;

			case omni::DICT:
				_ = ((Dict*) data[i]->_ptr)->_parent;

				break;
		}

		obj.push_back(_);
	}

	// prop
	for (const auto& entry : deser["prop"]) {
		Obj* _ = util::json::prop(entry);

		obj.push_back(_);
	}

	// bound
	boundRng._ptr = (Lim**) malloc(deser["bound"]["rng"].size() * sizeof (Lim*));
	boundRng._sz = deser["bound"]["rng"].size();

	boundArea._ptr = (Cone**) malloc(deser["bound"]["area"].size() * sizeof (Cone*));
	boundArea._sz = deser["bound"]["area"].size();

	for (const auto& entry : deser["bound"].items()) {
		int i = 0;
		if (entry.key() == "rng") {
			for (const auto& entry : entry.value()) {
				for (const auto& rng : entry) {
					for (const auto& lim : rng.items()) {
						Lim* _ = util::json::bound::lim(lim.key(), lim.value());

						((Lim**) boundRng._ptr)[i] = _;
						i++;

						line.push_back(_->_parent);
					}
				}	
			}	
		}

		if (entry.key() == "area") {
			int i = 0;
			for (const auto& entry : entry.value()) {
				Cone* _ = util::json::bound::area(entry);

				((Cone**) boundArea._ptr)[i] = _;
				i++;

				obj.push_back(_->_parent);
				pt.push_back(_->_parent->_child[0]);
			}
		}
	}

	// control flow
	streetLight._ptr = (StreetLight**) malloc(deser["ctrl"].size() * sizeof (StreetLight*));
	streetLight._sz = deser["ctrl"].size();

	int i = 0;
	for (const auto& entry : deser["ctrl"].items()) {
		StreetLight* _ = util::json::streetLight(entry.value());

		((StreetLight**) streetLight._ptr)[i] = _;
		i++;

		obj.push_back(_->_parent);
	}
}
