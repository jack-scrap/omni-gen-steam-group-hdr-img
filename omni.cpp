#include <iostream>

#include "omni.h"

void omni::err(unsigned int err, std::vector<std::string> arg) {
	std::string msg;
	switch (err) {
		case ERR_FS_RO:
			msg = "Couldn't write '" + arg[0] + "'; file read-only";

			break;

		case ERR_FS_NO_ENTRY:
			msg = "Entry '" + arg[0] + "' not found";

			break;

		case ERR_BUFF_DIFF:
			msg = "Buffer not saved";

			break;

		case ERR_FS_DEL_ENTRY:
			msg = "Couldn't delete entry '" + arg[0] + "'";

			break;

		case ERR_FS_OPEN_DIR:
			msg = "Couldn't open directory '" + arg[0] + "'";

			break;

		case ERR_FS_DIR_EXIST:
			msg = "Couldn't create directory '" + arg[0] + "'; already present";

			break;

		case ERR_FS_NO_PARENT_DIR:
			msg = "Couldn't write file '" + arg[0] + "'; parent directory `" + arg[1] + "` not present";

			break;

		case ERR_FS_ROOT_DIR:
			msg = "Couldn't traverse parent directory -- at root";

			break;

		case ERR_NO_CMD:
			msg = "Command `" + arg[0] + "` not found";

			break;

		case ERR_ARG_CNT: {
			std::string cmd = arg[0];

			std::string cand;

			cand += "[";

			for (int i = 0; i < lib[cmd].size(); i++) {
				cand += std::to_string(lib[cmd][i]);

				if (i < lib[cmd].size() - 1) {
					cand += ", ";
				}
			}

			cand += "]";

			msg = "Incorrect number of arguments to command `" + arg[0] + "`; expected one of " + cand;

			break;
		}

		case ERR_SCRIPT_NOT_FIN: {
			msg = "Cannot execute script; previous script still running";

			break;
		}

		case ERR_TOK:
			msg = "Inappropriate token '" + arg[0] + "'";

			break;

		case ERR_TOK_CNT:
			msg = "Inappropriate number of tokens in config entry";

			break;

		case ERR_CFG_NO_KEY:
			msg = "Config key '" + arg[0] + "' not found";

			break;

		case ERR_CFG_KEY:
			msg = "Inappropriate key '" + arg[0] + "', can only be alpha-numeric with `_` separator";

			break;

		case ERR_LVL_NOT_FIN:
			msg = "Couldn't proceed -- current level not complete";

			break;

		case ERR_SHADER:
			msg = "Shader compilation failed";

			break;

		case ERR_OBJ_CLIP:
			msg = "Cannot move; objects clipping";

			break;

		case ERR_MV_RNG:
			msg = "Cannot move " + arg[0] + "; translation exceeds range";

			break;

		case ERR_RD_TEX:
			msg = "Couldn't load texture " + arg[0];

			break;

		case ERR_FRAME_BUFF:
			msg = "Framebuffer not complete";

			break;
	}

	std::cerr << "Error: " << msg << std::endl;
}

void omni::assert(bool cond, std::string msg) {
	if (!cond) {
		std::cerr << "Assertion failed: " << msg << std::endl;

		exit(1);
	}
}
