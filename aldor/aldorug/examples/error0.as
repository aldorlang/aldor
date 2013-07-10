#include "aldor"

factorial(n: AldorInteger): AldorInteger == 
    n <= 1 => 1
    n * f(n-1)
