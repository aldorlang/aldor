-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 16-Feb-2006
-- Aldor version 1.0.3 for LINUX(glibc2.3)
-- Subject: Exception declaration in function signature

-- Compiling with "aldor aaa.as" works fine.
-- Adding the switches -DC1 and -DC2 results in compile errors:
-- 

-- (-DC1) That is understandable.
--:"aaa.as", line 11: } == add {
--:                   .....^
--:[L11 C6] #1 (Error) The domain is missing some exports.
--: 	Missing split: String -> ((String, String) throw SyntaxExceptionType)

-- (-DC2) That is understandable.
--:"aaa.as", line 11: } == add {
--:                   .....^
--:[L11 C6] #2 (Error) The domain is missing some exports.
--:	Missing split: String -> (String, String)
--:
--:"aaa.as", line 21:                 (a, b) := split("some string");
--:                   .................^..^.....^
--:[L21 C18] #3 (Error) No meaning for identifier `a'.
--:[L21 C21] #4 (Error) No meaning for identifier `b'.
--:[L21 C27] #1 (Error) This right hand side cannot be split for multiple assignment.

-- (-DC1 -DC2) That should compile without error.
--:"aaa.as", line 21:                 (a, b) := split("some string");
--:                   ..........................^
--:[L21 C27] #1 (Error) This right hand side cannot be split for multiple assignment.


#include "aldor"
#include "aldorio"

PkgA: with {
#if C1
	split: String -> (String, String) throw SyntaxExceptionType;
#else
	split: String -> (String, String);
#endif
	foo: () -> ();
} == add {
#if C2
	split(s: String): (String, String) throw SyntaxExceptionType == {
#else
	split(s: String): (String, String) == {
#endif
		empty? s => throw SyntaxException;
		("key", "value");
	}
	foo(): () == {
		a: String := "a";
		b: String := "b";
		(a, b) := split("some string");
	}
}
