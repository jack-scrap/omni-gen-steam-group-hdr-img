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

Arr* data;
std::vector<char> rhs;
bool eq = false;

Obj* test;

void scn::init(unsigned int i) {
	nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + std::to_string(i) + ".json"));

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
			mesh.push_back(crane->_parent);
		}

		if (entry["name"] == "truck") {
			Truck* truck = truckMk(glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["loc"][1], entry["loc"][2]));

			vehicle.push_back(truck);
			mesh.push_back(truck->_parent);
		}
	}

	// data
	for (const auto& pair : serial["data"].items()) {
		std::vector<char> init;

		for (const auto& item : serial["data"][pair.key()]) {
			init.push_back((char) ((int) item));
		}

		Node* child[] = {
			nodeMk(&init[0], init.size())
		};
		Node* node = nodeMk(&init[0], init.size(), child, sizeof child / sizeof *child);
		data = arrMk(node, pair.key());

		for (int i = 0; i < sizeof child / sizeof *child; i++) {
			free(child[i]);
		}
		free(node);
	}

	for (const auto& entry : serial["rhs"]) {
		rhs.push_back((char) ((int) entry));
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
