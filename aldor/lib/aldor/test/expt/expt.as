#include "aldor"
-- #include "algebra"

MI ==> MachineInteger ;

define CardType : Category == with {
    n : MI ;
}

super(X:CardType) : CardType == add {
    import from MI ;
    -- n == n$X^2 ; -- generates an error
    -- (Warning) Illegal recursive definition: <n:? == (n$X) ^ 2, n:? == (n$X) ^ 2>
    local y : MI := n$X^2 ;
    n == y ;
    ++ above definition of n allowed.
}    
