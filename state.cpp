#include "state.h"

unsigned int state::lineCnt = 0;
unsigned int state::lineWd = 0;

unsigned int state::baseNo = 1;

unsigned int state::hlLineNo = true;
unsigned int state::hlActiveLine = false;

unsigned int state::fps = 15;

unsigned int state::tabWd = 2;
bool state::expandTab = false;

unsigned int state::speed = 3;

std::string state::startDir = ".";

unsigned int state::showFilePerm = false;

unsigned int state::showDiff = false;

std::string state::format;
