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
			"dict"
		}, {
			3,
			"ctrl_flow"
		}, {
			4,
			"thread"
		}, {
			5,
			"path"
		}, {
			6,
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
}
