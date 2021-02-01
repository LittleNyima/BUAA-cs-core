#include "lib.h"
#define ARRAYSIZE (1024 * 10)
int bigarray[ARRAYSIZE] = { 0 };
void
umain(int argc, char** argv) {
	int i;
	writef("Making sure bss works right...\n");
	for (i = 0; i < ARRAYSIZE; i++)
		if (bigarray[i] != 0) user_panic("bigarray[%d] isnt cleared!\n", i);
	for (i = 0; i < ARRAYSIZE; i++)
		bigarray[i] = i;
	for (i = 0; i < ARRAYSIZE; i++)
		if (bigarray[i] != i) user_panic("bigarray[%d] didnt hold its val!\n", i);
	writef("well, now doing a wild write off the end...\n");
	bigarray[ARRAYSIZE + 1024] = 0;
	user_panic("should have trapped!");
}
