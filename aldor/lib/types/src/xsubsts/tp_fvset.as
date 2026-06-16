#include "types"
#include "aldorio"
#pile

-- Maybe make this an expression type (ie. unions, etc)
FVSet: SExpressionOutputType with
    fvset: SymbolMeaning -> %
    generator: % -> Generator SymbolMeaning
    empty: () -> %
    +: (%, %) -> %
== add    
    Rep == ListSet SymbolMeaning
    import from Rep, List SymbolMeaning, SymbolMeaning

    fvset(x: SymbolMeaning): % == per [x]
    empty(): % == per []
    
    (+)(a: %, b: %): % == per(rep(a) + rep(b))
    sexpression(fv: %): SExpression == [sexpression s for s in rep fv]
    generator(fv: %): Generator SymbolMeaning == generator rep fv
