#ifndef _TESTLIB_H_
#define _TESTLIB_H_

#include "axlgen.h"

void testStringEqual(String testName, String s1, String s2);
void testPointerEqual(String testName, void *, void *);
void testIntEqual(String testName, int s1, int s2);
void testIntIsNotZero(String testName, int s1);

void testTrue(String testName, Bool flg);
void testFalse(String testName, Bool flg);

void testIsNull(String testName, void *p);
void testIsNotNull(String testName, void *p);

void testShowSummary();
int testAllPassed();

void showTest(char *name, void (*fn)(void));

#define TEST(x) showTest(Enstring(x), x)

#endif
