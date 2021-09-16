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
		"del",
		"run",
		"next",
		"update",
		"quit"
	};

	static std::vector<std::string> var = {
		"skip_boot",
		"speed",
		"wd",
		"fps",
		"line_count",
		"line_width",
		"tab_wd",
		"time_format"
	};

	enum type {
		SCALAR,
		ARRAY,
		DICT
	};
}
