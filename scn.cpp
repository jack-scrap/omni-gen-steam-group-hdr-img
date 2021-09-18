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
#include "array.h"
#include "lim.h"
#include "cone.h"
#include "cargo_ship.h"
#include "omni.h"
#include "line.h"
#include "dict.h"
#include "street_sign.h"
#include "road.h"
#include "i_beam.h"
#include "util.h"

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

CArr crane = {
	(Crane**) malloc(0),
	0
};

CArr craneGet() {
	return crane;
}

CArr truck = {
	(Truck**) malloc(0),
	0
};

CArr truckGet() {
	return truck;
}

CArr cargoShip = {
	(CargoShip**) malloc(0),
	0
};

CArr cargoShipGet() {
	return cargoShip;
}

std::vector<Obj*> scn::obj;
std::vector<unsigned int> scn::prim;

CArr streetSign = {
	(StreetSign**) malloc(0),
	0
};

CArr streetSignGet() {
	return streetSign;
}

CArr boundRng = {
	(Lim**) malloc(0),
	0
};

CArr boundRngGet() {
	return boundRng;
}

CArr boundArea = {
	(Cone**) malloc(0),
	0
};

CArr boundAreaGet() {
	return boundArea;
}

CArr node = {
	(GLfloat*) malloc(0),
	0
};

CArr nodeGet() {
	return node;
}

CArr path = {
	(unsigned int*) malloc(0),
	0
};

CArr pathGet() {
	return path;
}

void scn::init(std::string stage, unsigned int lvl) {
	nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("lvl/" + stage + "/" + std::to_string(lvl) + ".json"));

	/* de-allocate */
	// data
	for (int i = 0; i < noData; i++) {
		switch (type[i]) {
			case omni::SCALAR:
				idxDel((Idx*) data[i]);
				idxDel((Idx*) goal[i]);

				break;

			case omni::ARRAY:
				arrayDel((Array*) data[i]);
				arrayDel((Array*) goal[i]);

				break;

			case omni::DICT:
				dictDel((Dict*) data[i]);
				dictDel((Dict*) goal[i]);

				break;
		}
	}

	// vehicle
	for (int i = 0; i < crane._sz; i += sizeof (Crane*)) {
		craneDel(((Crane**) crane._ptr)[i]);
	}

	for (int i = 0; i < truck._sz; i += sizeof (Truck*)) {
		truckDel(((Truck**) truck._ptr)[i]);
	}

	for (int i = 0; i < cargoShip._sz; i += sizeof (CargoShip*)) {
		cargoShipDel(((CargoShip**) cargoShip._ptr)[i]);
	}

	obj.clear();

	// bound
	for (int i = 0; i < boundRng._sz; i += sizeof (Lim*)) {
		limDel(((Lim**) boundRng._ptr)[i]);
	}

	for (int i = 0; i < boundArea._sz; i += sizeof (Cone*)) {
		coneDel(((Cone**) boundArea._ptr)[i]);
	}

	/* allocate */
	// data
	noData = deser["data"].size();

	type = (unsigned int*) malloc(noData * sizeof (unsigned int*));

	// initial
	Scope init = util::json::scope(deser["data"]);
	data = init._ptr;

	// desired
	Scope desired = util::json::scope(deser["goal"]);
	goal = desired._ptr;

	for (int i = 0; i < init._no; i++) {
		omni::assertion(init._type[i] == desired._type[i], std::string("Data `") + data[i]->_id + std::string("` not comparable"));
	}

	type = init._type;

	for (int i = 0; i < noData; i++) {
		Obj* _;

		switch (type[i]) {
			case omni::SCALAR:
				_ = ((Idx*) data[i]->_ptr)->_parent;

				break;

			case omni::ARRAY:
				_ = ((Array*) data[i]->_ptr)->_parent;

				break;

			case omni::DICT:
				_ = ((Dict*) data[i]->_ptr)->_parent;

				break;
		}

		obj.push_back(_);
		prim.push_back(Mesh::OBJ);
	}

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
			Cont* init;
			if (entry.contains("data")) {
				char c = util::json::byte(entry["data"]);
				init = contMk(c);
			} else {
				init = nullptr;
			}

			Crane* _ = craneMk(init, loc, rot);

			omni::assertion(!util::phys::collGround(_->_parent), "Crane clipping into ground plane");

			crane._sz += sizeof (Crane*);
			crane._ptr = (Crane**) realloc(crane._ptr, crane._sz);
			((Crane**) crane._ptr)[crane._sz - sizeof (Crane*)] = _;

			obj.push_back(_->_parent);
			prim.push_back(Mesh::OBJ);
		}

		if (entry["name"] == "cargo_ship") {
			CBuff array = util::json::array::array(entry["data"]["block"]);
			Array* init = arrayMk((char*) array._ptr, array._x, std::string(entry["data"]["name"]), X);

			CargoShip* _ = cargoShipMk(init, loc, rot);

			cargoShip._sz += sizeof (CargoShip*);
			cargoShip._ptr = (CargoShip**) realloc(cargoShip._ptr, cargoShip._sz);
			((CargoShip**) cargoShip._ptr)[cargoShip._sz - sizeof (CargoShip*)] = _;

			obj.push_back(_->_parent);
			prim.push_back(Mesh::OBJ);
		}

		if (entry["name"] == "truck") {
			CBuff init;
			if (entry.contains("data")) {
				init = util::json::array::array(entry["data"]);
			} else {
				init = {
					nullptr,
					0
				};
			}

			Array* array = arrayMk((char*) init._ptr, init._x, "", Z, glm::vec3(0.0, layout::pad * 2, -((layout::idx[X] / 2) + (layout::stroke * 2) + (layout::margin * 2 * 2))), glm::vec3(0.0, -M_PI / 2, 0.0));

			Truck* _ = truckMk(array, loc, rot);

			omni::assertion(!util::phys::collGround(_->_parent), "Truck clipping into ground plane");

			truck._sz += sizeof (Truck*);
			truck._ptr = (Truck**) realloc(truck._ptr, truck._sz);
			((Truck**) truck._ptr)[truck._sz - sizeof (Truck*)] = _;

			obj.push_back(_->_parent);
			prim.push_back(Mesh::OBJ);

			obj.push_back(_->_parent->_child[Truck::BED]);
			prim.push_back(Mesh::PT);

			obj.push_back(_->_parent->_child[Truck::OUTER]);
			prim.push_back(Mesh::PT);
		}
	}

	// path
	for (const auto& strip : deser["road"]["path"]["idc"]) {
		for (const auto& node : strip) {
			path._sz++;
			path._ptr = (unsigned int*) realloc(path._ptr, path._sz * sizeof (unsigned int));
			((unsigned int*) path._ptr)[path._sz - 1] = node;
		}
	}

	node._sz = 0;
	node._ptr = (unsigned int*) malloc(node._sz * sizeof (unsigned int));
	for (const nlohmann::json::array_t& strip : deser["road"]["path"]["idc"]) {
		std::vector<Obj*> _ = util::json::path(strip, deser["road"]["node"], deser["road"]["path"]["status"]);

		node._sz += 3;
		node._ptr = (unsigned int*) realloc(node._ptr, node._sz * sizeof (unsigned int));
		int a = 0;
		for (const nlohmann::json::number_unsigned_t& axis : strip) {
			((unsigned int*) node._ptr)[node._sz - 3 + a] = axis;
			
			a++;
		}

		for (Obj* seg : _) {
			obj.push_back(seg);
			prim.push_back(Mesh::LINE);
		}
	}

	// prop
	for (const nlohmann::json::object_t& entry : deser["prop"]["static"]) {
		Obj* _ = util::json::prop(entry);

		obj.push_back(_);
		prim.push_back(Mesh::OBJ);
	}

	for (nlohmann::json entry : deser["prop"]["dyna"]) {
		glm::vec3 loc = glm::vec3(0.0);
		if (entry.contains("loc")) {
			loc = util::json::vec(entry["loc"]);
		}

		glm::vec3 rot = glm::vec3(0.0);
		if (entry.contains("rot")) {
			rot = util::json::vec(entry["rot"]);
		}

		if (entry["name"] == "i_beam") {
			Obj* _ = iBeamMk(entry["sz"], entry["axis"], loc, rot);

			obj.push_back(_);
			prim.push_back(Mesh::LINE);
		}
	}

	// bound
	boundRng._sz = deser["bound"]["rng"].size();
	boundRng._ptr = (Lim**) malloc(boundRng._sz * sizeof (Lim*));

	boundArea._sz = deser["bound"]["area"].size();
	boundArea._ptr = (Cone**) malloc(boundArea._sz * sizeof (Cone*));

	for (const auto& entry : deser["bound"].items()) {
		int i = 0;
		if (entry.key() == "rng") {
			for (const auto& entry : entry.value()) {
				for (const auto& rng : entry) {
					Lim* _ = util::json::bound::lim(rng);

					((Lim**) boundRng._ptr)[i] = _;
					i++;

					obj.push_back(_->_parent);
					prim.push_back(Mesh::LINE);
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
				prim.push_back(Mesh::OBJ);

				obj.push_back(_->_parent->_child[0]);
				prim.push_back(Mesh::PT);
			}
		}
	}

	// control-flow
	streetSign._sz = deser["ctrl"].size();
	streetSign._ptr = (StreetSign**) malloc(streetSign._sz * sizeof (StreetSign*));

	int i = 0;
	for (const auto& entry : deser["ctrl"].items()) {
		StreetSign* _ = util::json::streetSign(entry.value());

		((StreetSign**) streetSign._ptr)[i] = _;
		i++;

		obj.push_back(_->_parent);
		prim.push_back(Mesh::OBJ);
	}
}
