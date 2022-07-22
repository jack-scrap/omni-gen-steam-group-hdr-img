#pragma once

#include <string>
#include <vector>
#include <map>

#undef assert

namespace omni {
	void err(unsigned int err, std::vector<std::string> arg = {});

	void assert(bool cond, std::string msg);

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

	static std::map<std::string, std::vector<unsigned int>> lib = {
		{
			"open", {
				1
			}
		}, {
			"new", {
				1
			}
		}, {
			"new_dir", {
				1
			}
		}, {
			"save", {
				0,
				1
			}
		}, {
			"del", {
				0,
				1
			}
		}, {
			"rename", {
				1,
				2
			}
		}, {
			"copy", {
				1,
				2
			}
		}, {
			"run", {
				0,
				1
			}
		}, {
			"set", {
				2
			}
		}, {
			"next", {
				0
			}
		}, {
			"quit", {
				0
			}
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
		IDX,
		ARRAY,
		DICT
	};

	enum {
		ERR_FS_RO,
		ERR_FS_NO_ENTRY,
		ERR_FS_DEL_ENTRY,
		ERR_FS_OPEN_DIR,
		ERR_FS_DIR_EXIST,
		ERR_FS_NO_PARENT_DIR,
		ERR_FS_ROOT_DIR,

		ERR_BUFF_DIFF,

		ERR_NO_CMD,
		ERR_ARG_CNT,
		ERR_SCRIPT_NOT_FIN,

		ERR_TOK,
		ERR_TOK_CNT,
		ERR_CFG_NO_KEY,
		ERR_CFG_KEY,

		ERR_LVL_NOT_FIN,
		ERR_RANK_LT,

		ERR_OBJ_CLIP,

		ERR_MV_RNG,

		ERR_SHADER,

		ERR_RD_TEX,

		ERR_FRAME_BUFF
	};
}
