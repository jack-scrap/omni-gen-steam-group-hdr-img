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
			"ctrl_flow"
		}, {
			2,
			"thread"
		}, {
			3,
			"path"
		}, {
			4,
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
