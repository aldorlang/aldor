#include "test_cimp.h"
#include "malloc.h"

Value valueNew(int v)
{
	V v = malloc(sizeof(*Value));
	v->value = v;
	return v;
}

Value valueFree(Value v)
{
	free(v);
}

void valueIncrement(Value v1, Value v2)
{
	v1->value += v2->value;
}

int valueAsInt(Value v)
{
	return v->value;
}
