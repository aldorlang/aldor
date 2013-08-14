#include "aldor"

MI ==> MachineInteger ;

FixedSizeType : Category == with {
    cardinality : MI ;
}

Pair(Left:FixedSizeType,Right:FixedSizeType) : FixedSizeType == add {
    cardinality : MI == cardinality$Left + cardinality $ Right ;
}

Decomposable : Category == with {
    decomposes? : Boolean ;
}

Pair(Left:Decomposable,Right:Decomposable) : Decomposable == add {
    decomposes? : Boolean == decomposes?$Left and decomposes?$Right ;
}

Indirect(Base:Join(Decomposable,FixedSizeType)) : Join(Decomposable,FixedSizeType) == add {
    decomposes? : Boolean == decomposes? $ Base ;
    cardinality : MI      == cardinality $ Base ;
}


