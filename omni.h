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
			5
		}, {
			"str",
			10
		}, {
			"dict",
			20
		}, {
			"matrix",
			30
		}, {
			"vec",
			45
		}, {
			"ctrl_flow",
			60
		}, {
			"path",
			80
		}, {
			"thread",
			100
		}
	};

	static std::map<std::string, unsigned int> lib = {
		{
			"open",
			1
		}, {
			"new",
			1
		}, {
			"new_dir",
			1
		}, {
			"run",
			0
		}, {
			"save",
			1
		}, {
			"del",
			1
		}, {
			"rename",
			2
		}, {
			"copy",
			2
		}, {
			"set",
			2
		}, {
			"next",
			0
		}, {
			"quit",
			0
		}
	};

	static std::vector<std::string> var = {
		"skip_boot",
		"screen_width",
		"fps",
		"speed",
		"anim",
		"begin_index_at_one",
		"line_count",
		"line_width",
		"tab_width",
		"tab_expand",
		"start_dir",
		"hl_line_no",
		"hl_active_line",
		"show_file_perm",
		"show_diff",
		"time_format"
	};

	enum {
		SCALAR,
		ARRAY,
		DICT
	};

	enum {
		ERR_FS_RO,
		ERR_FS_NO_FILE,
		ERR_FS_DEL_FILE,
		ERR_FS_OPEN_DIR,
		ERR_FS_DIR_EXIST,

		ERR_BUFF_DIFF,

		ERR_NO_CMD,
		ERR_ARG_CNT,

		ERR_TOK,
		ERR_TOK_CNT,
		ERR_CFG_NO_KEY,
		ERR_CFG_KEY,

		ERR_LVL_NOT_FIN,

		ERR_OBJ_CLIP,

		ERR_MV_RNG,

		ERR_SHADER,

		ERR_FRAME_BUFF
	};
}
