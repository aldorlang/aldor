#include "aldor"
#include "aldorio"
#pile

rfact(n: Integer): Integer == if n = 0 then 1 else n*rfact(n-1)

ifact(n: Integer): Integer == 
        i := 1
        while n > 1 repeat 
                i := i * n
                n := n - 1
        i
