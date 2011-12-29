#ifndef _TESTLIB_H_
#define _TESTLIB_H_

#include "axlgen.h"

void testStringEqual(String testName, String s1, String s2);
void testIntEqual(String testName, int s1, int s2);
void testTrue(String testName, Bool flg);

void testShowSummary();
int testAllPassed();

#endif
