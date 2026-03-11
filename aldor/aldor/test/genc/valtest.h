#ifndef _TEST_CIMP_H
#define _TEST_CIMP_H

typedef struct _PVal {
	int value;
} *Value;

Value valueNew(int v);
Value valueFree(Value v);
void valueIncrement(Value v1, Value v2);
int valueAsInt(Value v);

#endif
