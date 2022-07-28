#include <stdlib.h>

#include "c_buff.h"

void cArrDel(CArr* inst) {
	free(inst->ptr);

	free(inst);
}

void cBuffDel(CBuff* inst) {
	free(inst->ptr);

	free(inst);
}
