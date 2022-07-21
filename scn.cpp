#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <chrono>
#include "json.hpp"

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

Var** data = (Var**) malloc(0);
Var** goal = (Var**) malloc(0);
unsigned int noData = 0;

bool eq = false;

bool mtx = false;

bool adv = false;

Var** dataGet() {
	return data;
}

Var** goalGet() {
	return goal;
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

Obj** prop = (Obj**) malloc(0);
unsigned int noProp = 0;

std::vector<Obj*> scn::obj;

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

unsigned int call = 0;

void callIncr() {
	call++;
}

void scn::init(std::string stage, unsigned int lvl) {
	nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("lvl" + util::fs::path::sep + stage + util::fs::path::sep + std::to_string(lvl) + ".json"));

	/* de-allocate */
	// data
	for (int i = 0; i < noData; i++) {
		switch (data[i]->type) {
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
	for (int i = 0; i < crane.sz / sizeof (Crane*); i++) {
		craneDel(((Crane**) crane.ptr)[i]);
	}

	for (int i = 0; i < truck.sz / sizeof (Truck*); i++) {
		truckDel(((Truck**) truck.ptr)[i]);
	}

	for (int i = 0; i < cargoShip.sz / sizeof (CargoShip*); i++) {
		cargoShipDel(((CargoShip**) cargoShip.ptr)[i]);
	}

	obj.clear();

	// bound
	for (int i = 0; i < boundRng.sz / sizeof (Lim*); i++) {
		limDel(((Lim**) boundRng.ptr)[i]);
	}

	for (int i = 0; i < boundArea.sz / sizeof (Cone*); i++) {
		coneDel(((Cone**) boundArea.ptr)[i]);
	}

	for (int i = 0; i < noProp; i++) {
		objDel(prop[i]);
	}

	/* allocate */
	// data
	// LHS
	nlohmann::json scope = deser["data"];

	noData = scope.size();

	data = (Var**) realloc(data, noData * sizeof (Var*));
	goal = (Var**) realloc(goal, noData * sizeof (Var*));

	int i = 0;
	glm::vec3 offset = glm::vec3(0.0);
	for (const auto& pair : scope.items()) {
    char* name = util::json::id(pair.key());

    glm::vec3 loc = offset;
    if (pair.value().contains("loc")) {
      loc = util::json::vec(pair.value()["loc"]);
    }

    glm::vec3 rot = glm::vec3(0.0);
    if (pair.value().contains("rot")) {
      rot = util::json::vec(pair.value()["rot"]);
    }

    Var* item = util::json::var(pair.key(), pair.value(), loc, rot);

    GLfloat wd;
    switch (pair.value()["block"].type()) {
			// scalar
			case nlohmann::json::value_t::number_unsigned: {
				wd = ((Idx*) item->ptr)->_parent->_aabb[X][MAX] - ((Idx*) item->ptr)->_parent->_aabb[X][MIN];

				data[i] = item;

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				omni::assert(util::json::array::euclid(pair.value()["block"], 0), std::string("Depth of `") + pair.key() + std::string("` exceeds 3 dimensions"));

				switch (pair.value()["block"][0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						wd = ((Array*) item->ptr)->_parent->_aabb[X][MAX] - ((Array*) item->ptr)->_parent->_aabb[X][MIN];

						data[i] = item;

						break;
					}

					// 2D
					case nlohmann::json::value_t::array: {
						switch (pair.value()["block"][0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								wd = ((Array*) item->ptr)->_parent->_aabb[X][MAX] - ((Array*) item->ptr)->_parent->_aabb[X][MIN];

								data[i] = item;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								wd = ((Array*) item->ptr)->_parent->_aabb[X][MAX] - ((Array*) item->ptr)->_parent->_aabb[X][MIN];

								data[i] = item;

								break;
							}
						}

						break;
					}
				}

				break;
			}

			// string
			case nlohmann::json::value_t::string: {
				wd = ((Array*) item->ptr)->_parent->_aabb[X][MAX] - ((Array*) item->ptr)->_parent->_aabb[X][MIN];

				data[i] = item;

				break;
			}

			// dictionary
			case nlohmann::json::value_t::object: {
				wd = ((Dict*) item->ptr)->_parent->_aabb[X][MAX] - ((Dict*) item->ptr)->_parent->_aabb[X][MIN];

				data[i] = item;

				break;
			}
    }

    offset += glm::vec3(layout::padded(wd), 0.0, 0.0);

    i++;
  }

	// RHS
	nlohmann::json scopeRhs = deser["goal"];

	i = 0;
	for (const auto& pair : scopeRhs.items()) {
    char* name = util::json::id(pair.key());

    Var* item = util::json::var(pair.key(), pair.value());

		unsigned int typeRhs;

    GLfloat wd;
    switch (pair.value()["block"].type()) {
			// scalar
			case nlohmann::json::value_t::number_unsigned: {
				goal[i] = item;
				typeRhs = omni::SCALAR;

				break;
			}

			// string
			case nlohmann::json::value_t::string: {
				goal[i] = item;
				typeRhs = omni::ARRAY;

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				omni::assert(util::json::array::euclid(pair.value()["block"], 0), std::string("Depth of `") + pair.key() + std::string("` exceeds 3 dimensions"));

				switch (pair.value()["block"][0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						goal[i] = item;
						typeRhs = omni::ARRAY;

						break;
					}

					// 2D
					case nlohmann::json::value_t::array: {
						switch (pair.value()["block"][0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								goal[i] = item;
								typeRhs = omni::ARRAY;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								goal[i] = item;
								typeRhs = omni::ARRAY;

								break;
							}
						}

						break;
					}
				}

				break;
			}

			// dictionary
			case nlohmann::json::value_t::object: {
				goal[i] = item;
				typeRhs = omni::DICT;

				break;
			}
    }

		omni::assert(typeRhs == data[i]->type, "Can't compare data; types not comparable");

    i++;
  }

	for (int i = 0; i < noData; i++) {
		Obj* instObj;

		switch (data[i]->type) {
			case omni::SCALAR:
				instObj = ((Idx*) data[i]->ptr)->_parent;

				break;

			case omni::ARRAY:
				instObj = ((Array*) data[i]->ptr)->_parent;

				break;

			case omni::DICT:
				instObj = ((Dict*) data[i]->ptr)->_parent;

				break;
		}

		obj.push_back(instObj);
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
			Cont* init = nullptr;
			if (entry.contains("data")) {
				char c = util::json::byte(entry["data"]);

				init = contMk(c);
			}

			Crane* instCrane = craneMk(init, loc, glm::radians(rot));

			omni::assert(!util::phys::aabbGround(instCrane->_parent), "Crane clipping into ground plane");

			crane.sz += sizeof (Crane*);
			crane.ptr = (Crane**) realloc(crane.ptr, crane.sz);
			((Crane**) crane.ptr)[crane.sz - sizeof (Crane*)] = instCrane;

			obj.push_back(instCrane->_parent);
		}

		if (entry["name"] == "truck") {
			CBuff init;
			if (entry.contains("data")) {
				init = util::json::array::array(entry["data"]);
			} else {
				init = {
					(char*) malloc(1),
					1,
					0,
					0
				};

				((char*) init.ptr)[0] = 0;
			}

			Array* instArray = arrayMk((char*) init.ptr, init.x, "", Z, glm::vec3(0.0, layout::padded(0.0), -((layout::idx[X] / 2) + (layout::stroke * 2) + (layout::margin * 2 * 2))), glm::vec3(0.0, -M_PI / 2, 0.0));

			Truck* instTruck = truckMk(instArray, loc, glm::radians(rot));

			omni::assert(!util::phys::aabbGround(instTruck->_parent), "Truck clipping into ground plane");

			truck.sz += sizeof (Truck*);
			truck.ptr = (Truck**) realloc(truck.ptr, truck.sz);
			((Truck**) truck.ptr)[truck.sz - sizeof (Truck*)] = instTruck;

			obj.push_back(instTruck->_parent);

			obj.push_back(instTruck->_parent->_child[Truck::BED]);

			obj.push_back(instTruck->_parent->_child[Truck::OUTER]);
		}

		if (entry["name"] == "cargo_ship") {
			std::string name;

			CBuff init;
			if (entry.contains("data")) {
				init = util::json::array::array(entry["data"]["block"]);

				if (entry["data"].contains("name")) {
					name = std::string(entry["data"]["name"]);
				}
			} else {
				init = {
					(char*) malloc(6 * 2),
					6,
					2,
					0
				};

				for (int j = 0; j < init.x; j++) {
					for (int i = 0; i < init.y; i++) {
						((char*) init.ptr)[(j * init.y) + i] = 0;
					}
				}
			}

			Array* instArray = arrayMk((char*) init.ptr, init.x, init.y, name, glm::vec3(0.0, layout::padded(0.0), 0.0));

			CargoShip* instCargoShip = cargoShipMk(instArray, loc, glm::radians(rot));

			cargoShip.sz += sizeof (CargoShip*);
			cargoShip.ptr = (CargoShip**) realloc(cargoShip.ptr, cargoShip.sz);
			((CargoShip**) cargoShip.ptr)[cargoShip.sz - sizeof (CargoShip*)] = instCargoShip;

			obj.push_back(instCargoShip->_parent);
		}
	}

	// path
	for (const auto& strip : deser["road"]["path"]["idc"]) {
		for (const auto& node : strip) {
			path.sz++;
			path.ptr = (unsigned int*) realloc(path.ptr, path.sz * sizeof (unsigned int));
			((unsigned int*) path.ptr)[path.sz - 1] = node;
		}
	}

	node.sz = 0;
	node.ptr = (unsigned int*) malloc(node.sz * sizeof (unsigned int));
	for (const nlohmann::json::array_t& strip : deser["road"]["path"]["idc"]) {
		std::vector<Obj*> path = util::json::path(strip, deser["road"]["node"], deser["road"]["path"]["status"]);

		node.sz += 3;
		node.ptr = (unsigned int*) realloc(node.ptr, node.sz * sizeof (unsigned int));
		int a = 0;
		for (const nlohmann::json::number_unsigned_t& axis : strip) {
			((unsigned int*) node.ptr)[node.sz - 3 + a] = axis;
			
			a++;
		}

		for (const Obj* seg : path) {
			obj.push_back((Obj*) seg);
		}
	}

	// prop
	noProp = deser["prop"]["static"].size();
	prop = (Obj**) realloc(prop, noProp * sizeof (Obj*));
	i = 0;
	for (const nlohmann::json::object_t& entry : deser["prop"]["static"]) {
		Obj* instObj = util::json::prop(entry);

		prop[i] = instObj;

		obj.push_back(instObj);

		i++;
	}

	for (const nlohmann::json entry : deser["prop"]["dyna"]) {
		glm::vec3 loc = glm::vec3(0.0);
		if (entry.contains("loc")) {
			loc = util::json::vec(entry["loc"]);
		}

		glm::vec3 rot = glm::vec3(0.0);
		if (entry.contains("rot")) {
			rot = util::json::vec(entry["rot"]);
		}

		if (entry["name"] == "i_beam") {
			Obj* instObj = iBeamMk(entry["sz"], entry["axis"], loc, glm::radians(rot));

			obj.push_back(instObj);
		}
	}

	// mark
	for (int i = 0; i < deser["mark"].size(); i++) {
		Mark* instMark = util::json::bound::mark(deser["mark"][i]);

		obj.push_back(instMark->_parent);
	}

	// bound
	boundRng.sz = deser["bound"]["rng"].size() * sizeof (Lim*);
	boundRng.ptr = (Lim**) malloc(boundRng.sz);

	boundArea.sz = deser["bound"]["area"].size() * sizeof (Cone*);
	boundArea.ptr = (Cone**) malloc(boundArea.sz);

	for (const auto& entry : deser["bound"].items()) {
		int i = 0;
		if (entry.key() == "rng") {
			for (const auto& entry : entry.value()) {
				for (const auto& rng : entry) {
					Lim* instLim = util::json::bound::lim(rng);

					((Lim**) boundRng.ptr)[i] = instLim;
					i++;

					obj.push_back(instLim->_parent);
				}	
			}	
		}

		if (entry.key() == "area") {
			int i = 0;
			for (const auto& entry : entry.value()) {
				Cone* instCone = util::json::bound::area(entry);

				((Cone**) boundArea.ptr)[i] = instCone;
				i++;

				obj.push_back(instCone->_parent);

				obj.push_back(instCone->_parent->_child[0]);
			}
		}
	}

	// control-flow
	streetSign.sz = deser["ctrl"].size() * sizeof (StreetSign*);
	streetSign.ptr = (StreetSign**) malloc(streetSign.sz);

	i = 0;
	for (const auto& entry : deser["ctrl"].items()) {
		StreetSign* instStreetSign = util::json::streetSign(entry.value());

		((StreetSign**) streetSign.ptr)[i] = instStreetSign;
		i++;

		obj.push_back(instStreetSign->_parent);
	}

	/* general */
	call = 0;
}
