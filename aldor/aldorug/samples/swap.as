#include "aldor"
#include "aldorio"
#pile

I  ==> MachineInteger;
Ag ==> (S: Type) -> BoundedFiniteLinearStructureType S;

-- This function takes two type constructors as arguments and
-- produces a new function to swap aggregate data structure layers.

swap(X:Ag,Y:Ag)(S:Type)(x:X Y S):Y X S == 
    import from Y S, X S
    [[s for s in y]for y in x]

import from I, List(I);

-- Form an array of lists:
al: Array List I := [[i+j-1 for i in 1..3] for j in 1..3]

stdout << "This is an array of lists: " << newline
stdout << al << newline << newline

-- Swap the structure layers:

la: List Array I := swap(Array,List)(I)(al)

stdout << "This is a list of arrays:  " << newline
stdout << la << newline
