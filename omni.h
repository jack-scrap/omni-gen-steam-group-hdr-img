#pragma once

#include <string>
#include <vector>
#include <map>

namespace omni {
	void err(std::string msg);

	void assertion(bool cond, std::string msg);

	static std::map<std::string, unsigned int> stage = {
		{
			"init",
				0
		}, {
			"array",
				1
		}, {
			"str",
			2
		}, {
			"dict",
			3
		}, {
			"matrix",
			4
		}, {
			"vec",
			5
		}, {
			"ctrl_flow",
			6
		}, {
			"path",
			7
		}, {
			"thread",
			8
		}
	};

	static std::vector<std::string> lib = {
		"open",
		"set",
		"save",
		"run",
		"next",
		"update",
		"quit"
	};

	enum type {
		SCALAR,
		ARRAY,
		DICT
	};
}
