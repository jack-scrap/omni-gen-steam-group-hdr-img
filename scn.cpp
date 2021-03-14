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

Cam cam = {
	{
		-1000.0, 1000.0, 1000.0
	}, {
		50, 50, 50
	}, {
		-1000.0, 1000.0, -1000.0
	}
};

Disp disp("Omni", {
	layout::view[X] + (state::ln * layout::dim[X]), layout::view[Y]
}, col[false]);

unsigned int rank = 0;

void ld(unsigned int i) {
	nlohmann::json serial = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + std::to_string(i) + ".json"));

	// vehicles
	for (void* _ : vehicle) {
		free(_);
	}
	vehicle.clear();

	// drawable objects
	for (void* _ : scn) {
		free(_);
	}
	scn.clear();

	for (const auto& entry : serial["vehicle"]) {
		if (entry["name"] == "crane") {
			Crane* crane = craneMk(glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["loc"][1], entry["loc"][2]));

			vehicle.push_back(crane);
			scn.push_back(crane->_parent);
		}

		if (entry["name"] == "truck") {
			Truck* truck = truckMk(glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["loc"][1], entry["loc"][2]));

			vehicle.push_back(truck);
			scn.push_back(truck->_parent);
		}
	}

	// data
	std::vector<char> init;
	for (const auto& entry : serial["data"]) {
		init.push_back((char) ((int) entry));
	}
	Node* child[] = {
		nodeMk(&init[0], init.size())
	};
	Node* node = nodeMk(&init[0], init.size(), child, sizeof child / sizeof *child);
	data = arrMk(node, "data");

	// object
	for (const auto& entry : serial["obj"]) {
		Obj* obj = objMk(entry["name"], "obj", "dir", true, glm::vec3(entry["loc"][0], entry["loc"][1], entry["loc"][2]), glm::vec3(entry["rot"][0], entry["rot"][1], entry["rot"][2]));

		scn.push_back(obj);
	}

	scn.push_back(data->_parent);

	Lim* rng[2];
	for (int i = 0; i < 2; i++) {
		rng[i] = limMk(glm::vec3(0.0, 0.0, -layout::stroke + ((i ? 1 : -1) * (10.0 + (layout::stroke * 2 * 2) + layout::stroke))), glm::vec3(M_PI / 2, 0.0, M_PI / 2));
	}

	for (int i = 0; i < 2; i++) {
		scn.push_back(rng[i]->_parent);
	}
}

char init[] = {
	0,
	'a',
	'a'
};
Arr* data;
char rhs[] = {
	'a',
	0,
	'a'
};
bool eq = false;

std::vector<void*> vehicle;
extern "C" void** vehicleGet() {
	return &vehicle[0];
}

std::vector<Obj*> scn;
