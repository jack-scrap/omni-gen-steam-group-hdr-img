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
#include "omni.h"

Arr* data;
std::vector<char> rhs;
bool eq = false;

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
			Crane* crane = craneMk(glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["loc"][1], entry["loc"][2]));

			vehicle.push_back(crane);
			/* mesh.push_back(crane->_parent); */
		}

		if (entry["name"] == "truck") {
			Truck* truck = truckMk(glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["loc"][1], entry["loc"][2]));

			vehicle.push_back(truck);
			mesh.push_back(truck->_parent);
		}
	}

	// data
	for (const auto& pair : serial["data"].items()) {
		// 1D
		/* char* init = (char*) malloc(0); */
		/* unsigned int sz = 0; */

		/* for (const auto& item : serial["data"][pair.key()]) { */
		/* 	if (item.type() == nlohmann::json::value_t::number_unsigned) { */
		/* 		sz++; */

		/* 		init = (char*) realloc(init, sz * sizeof (char)); */
		/* 		init[sz - 1] = (char) ((int) item); */
		/* 	} */
		/* } */

		/* data = arrMk(init, sz, pair.key()); */

		// 2D
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

		data = arrMk(init, x, y, pair.key());
	}

	for (const auto& entry : serial["rhs"]) {
		if (entry.type() == nlohmann::json::value_t::number_unsigned) {
			rhs.push_back((char) ((int) entry));
		}

		if (entry.type() == nlohmann::json::value_t::array) {
			for (const auto& byte : entry) {
				rhs.push_back((char) ((int) byte));
			}
		}
	}

	// object
	for (const auto& entry : serial["obj"]) {
		Obj* obj = objMk(entry["name"], "obj", "dir", true, glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["rot"][1], entry["rot"][2]));

		mesh.push_back(obj);
	}

	// bound
	for (const auto& entry : serial["bound"].items()) {
		if (entry.key() == "rng") {
			for (const auto& _ : entry.value().items()) {
				Lim* lim = limMk(glm::vec3(_.value()["loc"][0], _.value()["loc"][1], _.value()["loc"][2]), glm::vec3(_.value()["rot"][0], _.value()["rot"][1], _.value()["rot"][2]));

				mesh.push_back(lim->_parent);
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
