#pragma once

#include <string>
#include <vector>
#include <map>

namespace omni {
	void err(unsigned int err, std::vector<std::string> arg = {});

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
		"new",
		"new_dir",
		"run",
		"save",
		"del",
		"next",
		"quit"
	};

	static std::vector<std::string> var = {
		"skip_boot",
		"speed",
		"screen_wd",
		"fps",
		"begin_index_at_one",
		"line_count",
		"line_width",
		"tab_wd",
		"hl_line_no",
		"hl_active_line",
		"time_format"
	};

	enum {
		SCALAR,
		ARRAY,
		DICT
	};

	enum {
		ERR_FS_RO,
		ERR_FS_OPEN_FILE,
		ERR_FS_DEL_FILE,
		ERR_FS_OPEN_DIR,
		ERR_FS_DIR_EXIST,

		ERR_BUFF_DIFF,

		ERR_TOK,
		ERR_TOK_CNT,
		ERR_CFG_KEY,

		ERR_NO_CMD,
		ERR_ARG_CNT,

		ERR_LVL_NOT_FIN,

		ERR_OBJ_CLIP,

		ERR_MV_RNG,

		ERR_SHADER,

		ERR_FRAME_BUFF
	};
}
