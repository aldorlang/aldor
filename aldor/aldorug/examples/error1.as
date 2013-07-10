#include "aldor"
#pile

factorial(n: Integer): Integer == 
    n <= 1 => 1
    n * f(n-1)
