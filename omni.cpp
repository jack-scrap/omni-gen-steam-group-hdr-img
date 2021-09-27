#include <iostream>

#include "omni.h"

void omni::err(unsigned int err, std::vector<std::string> arg) {
	std::string msg;
	switch (err) {
		case ERR_FS_RO:
			msg = "Could not write `" + arg[0] + "`; file read-only";

			break;

		case ERR_FS_OPEN_FILE:
			msg = "Could not open `" + arg[0] + "`; file not found";

			break;

		case ERR_BUFF_DIFF:
			msg = "Buffer not saved";

			break;

		case ERR_FS_DEL_FILE:
			msg = "Could not delete file `" + arg[0] + "`";

			break;

		case ERR_FS_OPEN_DIR:
			msg = "Could not open directory `" + arg[0] + "`";

			break;

		case ERR_FS_DIR_EXIST:
			msg = "Could not create directory `" + arg[0] + "`; already present";

			break;

		case ERR_TOK_CNT:
			msg = "Inappropriate number of tokens in config entry";

			break;

		case ERR_TOK:
			msg = "Inappropriate token `" + arg[0] + "`";

			break;

		case ERR_CFG_KEY:
			msg = "Inappropriate key `" + arg[0] + "`, can only be alpha-numeric with `_` separator";

			break;

		case ERR_NO_CMD:
			msg = "Command `" + arg[0] + "` not found";

			break;

		case ERR_ARG_CNT:
			msg = "Incorrect number of arguments to command `" + arg[0] + "`";

			break;

		case ERR_LVL_NOT_FIN:
			msg = "Could not proceed -- current level not complete";

			break;

		case ERR_SHADER:
			msg = "Shader compilation failed";

			break;

		case ERR_OBJ_CLIP:
			msg = "Cannot move; objects clipping";

			break;

		case ERR_CRANE_HEAD_RNG:
			msg = "Cannot move crane head; translation exceeds range";

			break;

		case ERR_CRANE_CLAW_RNG:
			msg = "Cannot move crane claw; translation exceeds range";

			break;

		case ERR_TRUCK_WHEEL_RNG:
			msg = "Cannot turn truck; rotation of wheels exceeds range";

			break;

		case ERR_FRAME_BUFF:
			msg = "Framebuffer not complete";

			break;
	}

	std::cout << "Error: " << msg << std::endl;
}

void omni::assertion(bool cond, std::string msg) {
	if (!cond) {
		std::cout << "Assertion failed: " << msg << std::endl;

		exit(1);
	}
}
