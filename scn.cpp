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
unsigned int* type = (unsigned int*) malloc(0);
unsigned int noData = 0;

bool eq = false;

bool mtx = false;

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

Obj** prop = (Obj**) malloc(0);
unsigned int noProp = 0;

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

unsigned int c = 0;

void cInc() {
	c++;
}

void scn::init(std::string stage, unsigned int lvl) {
	nlohmann::json deser = nlohmann::json::parse(util::fs::rd<std::string>("lvl" + util::fs::path::sep + stage + util::fs::path::sep + std::to_string(lvl) + ".json"));

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
	for (int i = 0; i < crane._sz / sizeof (Crane*); i++) {
		craneDel(((Crane**) crane._ptr)[i]);
	}

	for (int i = 0; i < truck._sz / sizeof (Truck*); i++) {
		truckDel(((Truck**) truck._ptr)[i]);
	}

	for (int i = 0; i < cargoShip._sz / sizeof (CargoShip*); i++) {
		cargoShipDel(((CargoShip**) cargoShip._ptr)[i]);
	}

	obj.clear();

	// bound
	for (int i = 0; i < boundRng._sz / sizeof (Lim*); i++) {
		limDel(((Lim**) boundRng._ptr)[i]);
	}

	for (int i = 0; i < boundArea._sz / sizeof (Cone*); i++) {
		coneDel(((Cone**) boundArea._ptr)[i]);
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
	type = (unsigned int*) realloc(type, noData * sizeof (unsigned int));

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
				wd = ((Idx*) item->_ptr)->_parent->_aabb[X][MAX] - ((Idx*) item->_ptr)->_parent->_aabb[X][MIN];

				data[i] = item;
				type[i] = omni::SCALAR;

				break;
			}

			// array
			case nlohmann::json::value_t::array: {
				omni::assert(util::json::array::euclid(pair.value()["block"], 0), std::string("Depth of `") + pair.key() + std::string("` exceeds 3 dimensions"));

				switch (pair.value()["block"][0].type()) {
					// 1D
					case nlohmann::json::value_t::number_unsigned: {
						wd = ((Array*) item->_ptr)->_parent->_aabb[X][MAX] - ((Array*) item->_ptr)->_parent->_aabb[X][MIN];

						data[i] = item;
						type[i] = omni::ARRAY;

						break;
					}

					// matrix
					case nlohmann::json::value_t::array: {
						switch (pair.value()["block"][0][0].type()) {
							// 2D
							case nlohmann::json::value_t::number_unsigned: {
								wd = ((Array*) item->_ptr)->_parent->_aabb[X][MAX] - ((Array*) item->_ptr)->_parent->_aabb[X][MIN];

								data[i] = item;
								type[i] = omni::ARRAY;

								break;
							}

							// 3D
							case nlohmann::json::value_t::array: {
								wd = ((Array*) item->_ptr)->_parent->_aabb[X][MAX] - ((Array*) item->_ptr)->_parent->_aabb[X][MIN];

								data[i] = item;
								type[i] = omni::ARRAY;

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
				wd = ((Array*) item->_ptr)->_parent->_aabb[X][MAX] - ((Array*) item->_ptr)->_parent->_aabb[X][MIN];

				data[i] = item;
				type[i] = omni::ARRAY;

				break;
			}

			// dictionary
			case nlohmann::json::value_t::object: {
				wd = ((Dict*) item->_ptr)->_parent->_aabb[X][MAX] - ((Dict*) item->_ptr)->_parent->_aabb[X][MIN];

				data[i] = item;
				type[i] = omni::DICT;

				break;
			}
    }

    offset += glm::vec3(layout::padded(wd), 0.0, 0.0);

    i++;
  }

	// RHS
	nlohmann::json scopeGoal = deser["goal"];

	i = 0;
	for (const auto& pair : scopeGoal.items()) {
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

					// matrix
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

		omni::assert(typeRhs == type[i], "Can't compare data; types not comparable");

    i++;
  }

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
			Cont* init = nullptr;
			if (entry.contains("data")) {
				char c = util::json::byte(entry["data"]);

				init = contMk(c);
			}

			Crane* _ = craneMk(init, loc, rot);

			omni::assert(!util::phys::aabbGround(_->_parent), "Crane clipping into ground plane");

			crane._sz += sizeof (Crane*);
			crane._ptr = (Crane**) realloc(crane._ptr, crane._sz);
			((Crane**) crane._ptr)[crane._sz - sizeof (Crane*)] = _;

			obj.push_back(_->_parent);
			prim.push_back(Mesh::OBJ);
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

				for (int j = 0; j < 6; j++) {
					for (int i = 0; i < 2; i++) {
						((char*) init._ptr)[(j * 2) + i] = 0;
					}
				}
			}

			Array* array = arrayMk((char*) init._ptr, init._x, init._y, name, glm::vec3(0.0, layout::padded(0.0), 0.0));

			CargoShip* _ = cargoShipMk(array, loc, rot);

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
					(char*) malloc(1),
					1,
					0,
					0
				};

				((char*) init._ptr)[0] = 0;
			}

			Array* array = arrayMk((char*) init._ptr, init._x, "", Z, glm::vec3(0.0, layout::padded(0.0), -((layout::idx[X] / 2) + (layout::stroke * 2) + (layout::margin * 2 * 2))), glm::vec3(0.0, -M_PI / 2, 0.0));

			Truck* _ = truckMk(array, loc, rot);

			omni::assert(!util::phys::aabbGround(_->_parent), "Truck clipping into ground plane");

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
	prop = (Obj**) realloc(prop, deser["prop"]["static"].size() * sizeof (Obj*));
	for (const nlohmann::json::object_t& entry : deser["prop"]["static"]) {
		Obj* _ = util::json::prop(entry);

		prop[noProp] = _;
		noProp++;

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

	// mark
	for (int i = 0; i < deser["mark"].size(); i++) {
		Mark* _ = util::json::bound::mark(deser["mark"][i]);

		obj.push_back(_->_parent);
		prim.push_back(Mesh::LINE);
	}

	// bound
	boundRng._sz = deser["bound"]["rng"].size() * sizeof (Lim*);
	boundRng._ptr = (Lim**) malloc(boundRng._sz);

	boundArea._sz = deser["bound"]["area"].size() * sizeof (Cone*);
	boundArea._ptr = (Cone**) malloc(boundArea._sz);

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
	streetSign._sz = deser["ctrl"].size() * sizeof (StreetSign*);
	streetSign._ptr = (StreetSign**) malloc(streetSign._sz);

	i = 0;
	for (const auto& entry : deser["ctrl"].items()) {
		StreetSign* _ = util::json::streetSign(entry.value());

		((StreetSign**) streetSign._ptr)[i] = _;
		i++;

		obj.push_back(_->_parent);
		prim.push_back(Mesh::OBJ);
	}

	/* general */
	c = 0;
}
