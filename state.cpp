#include "state.h"

unsigned int state::lineCnt;
unsigned int state::lineWd;

unsigned int state::baseNo = 0;

unsigned int state::hlLineNo = true;
unsigned int state::hlActiveLine;

unsigned int state::fps;

unsigned int state::tabWd = 2;
bool state::expandTab = false;

unsigned int state::speed = 3;

std::string state::format;
