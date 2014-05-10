#include <stdlib.h>
#include "btree.h"

int main(int argc, char* argv[])
{
	BTree t = btreeNew(1);
	btreeFree(t);
	return 0;
}
