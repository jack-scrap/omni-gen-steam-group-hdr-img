#include <iostream>

#include "omni.h"

void omni::err(unsigned int err) {
	std::string msg;
	switch (err) {
		case ERR_FS_RO:
			msg = "Could not write; file read-only";

			break;

		case ERR_FS_OPEN_FILE:
			msg = "Could not open; file not found";

			break;

		case ERR_BUFF_DIFF:
			msg = "Buffer not saved";

			break;

		case ERR_FS_DEL_FILE:
			msg = "Could not delete file";

			break;

		case ERR_FS_OPEN_DIR:
			msg = "Could not open directory";

			break;

		case ERR_TOK_CNT:
			msg = "Inappropriate number of tokens in config entry";

			break;

		case ERR_TOK:
			msg = "Inappropriate token";

			break;

		case ERR_CFG_KEY:
			msg = "Inappropriate key, can only be alpha-numeric with `_` separator";

			break;

		case ERR_NO_CMD:
			msg = "Command not found";

			break;

		case ERR_ARG_CNT:
			msg = "Incorrect number of arguments to command";

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
