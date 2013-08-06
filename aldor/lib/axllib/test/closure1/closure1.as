#include "axllib"

import from SingleInteger, String;

foo(o: SingleInteger)(s: String): () == print << s << ": " << o << newline;

i := 0;
bar(): String -> () == {
	free i := i + 1;
	foo i
}

a := bar();
b := bar();
a "Hello";
b "Hello";
