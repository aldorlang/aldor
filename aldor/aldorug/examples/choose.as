--
-- choose.as: A file providing functions to be used elsewhere.
--
#include "aldor"

factorial(n: Integer): Integer ==
        if n <= 2 then n else n * factorial(n-1);

choose(n: Integer, k: Integer): Integer ==
        factorial(n) quo (factorial(k) * factorial(n-k));
