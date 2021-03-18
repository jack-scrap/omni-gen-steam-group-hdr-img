#pragma once

#include <string>
#include <vector>

namespace omni {
	void err(std::string msg);

	static std::vector<std::string> lib = {
		"open",
		"run",
		"save",
		"next"
	};
}
