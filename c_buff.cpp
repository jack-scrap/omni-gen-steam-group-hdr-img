#include <stdlib.h>

#include "c_buff.h"

void cArrDel(CArr* inst) {
	free(inst->ptr);
}

void cBuffDel(CBuff* inst) {
	free(inst->ptr);
}
