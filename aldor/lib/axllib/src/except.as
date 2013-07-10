#include "axllib"

+++ `Exception' is a basic category to which most exception types belong.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: exception

define Exception: Category == with {}

+++ `ArithmeticException' is a basic category to which most exception types belong.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: exception

define ArithmeticException: Category == Exception with {
	domain:      () -> BasicType;
	description: () -> String;
}

+++ `ArithmeticError' is a basic error type suitable for all sorts of 
+++ numeric exceptions.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: exception constructor

ArithmeticError(R: BasicType, desc: String): ArithmeticException with == add {
	domain(): BasicType == R;
	description(): String == desc; 
}

+++ `RangeException' is a specialised ArithmeticException which 
+++ holds information about an exception associated with bad range
+++ for a function.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: exception
define RangeException: Category == ArithmeticException with {
	function: 	() -> String;
	value: 		() -> (T: BasicType, T);
}


+++ `RangeError' is a basic error type raised where an argument is not in the 
+++ appropriate range for a function. (div by zero, log, etc).
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: exception

RangeError(X: BasicType, v: X, s: String): RangeException with == add {
	function(): String == s;
	value():    (T: BasicType, T) == (X, v);

	domain(): BasicType == X;
	description(): String == "Range Error";
}


define FileException: Category == Exception with {
	description: () -> String;
}

+++ `FileError' is a basic error type raised where a file-related function failed
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: exception

FileError(s: String): FileException with == add {
	description(): String == s;
}
