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
			0,
			"array"
		}, {
			2,
			"ctrl_flow"
		}, {
			3,
			"thread"
		}, {
			4,
			"path"
		}, {
			5,
			"matrix"
		}
	};

	static std::vector<std::string> lib = {
		"open",
		"run",
		"save",
		"next"
	};
}
