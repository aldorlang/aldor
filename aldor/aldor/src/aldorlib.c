#include <dirent.h>
#include "aldorlib.h"

FiPtr direntName(FiPtr ptr)
{
	struct dirent *ent = (struct dirent *) ptr;
	return ent->d_name;
}
