#include <stdio.h>

#include "asdf.h"
#include "hjkl.h"

int main() {
	Asdf asdf = asdfMk();

	printf("%d\n", asdf._asdf);

	hjkl();

	return 0;
}
