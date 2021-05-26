#pragma once

#include <string>
#include <vector>
#include <map>

namespace omni {
	void err(std::string msg);

	static std::map<unsigned int, std::string> stage = {
		{
			0,
			"init"
		}, {
			1,
			"array"
		}, {
			2,
			"str",
		}, {
			3,
			"dict"
		}, {
			4,
			"ctrl_flow"
		}, {
			5,
			"thread"
		}, {
			6,
			"path"
		}, {
			7,
			"matrix"
		}
	};

	static std::vector<std::string> lib = {
		"open",
		"set",
		"save",
		"run",
		"next"
	};

	enum type {
		SCALAR,
		ARRAY,
		DICT
	};
}
