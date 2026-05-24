#include "types"
#include "aldorio"
#pile

ParamTypeTerm: Join(PrimitiveType, SExpressionOutputType) with
    typeTerm: % -> TypeTerm
    paramSet: % -> ParamSet
    fv: % -> ListSet Symbol
    withType: (%, Expression) -> %
    withType: (%, TypeTerm) -> %
    withParams: (%, ParamSet) -> %
    varRef: (Symbol, Symbol) -> %
    asParam: TypeTerm -> % -- !!FIXME: Rename
== add
    Rep == Record(fv: ListSet Symbol, tt: Expression, ps: ParamSet)
    import from Rep
    import from Symbol
    import from TypeTerm
    import from ParamSet
    import from ListSet Symbol
    
    typeTerm(ptt: %): TypeTerm == forall(fv ptt, rep(ptt).tt)
    paramSet(ptt: %): ParamSet == rep(ptt).ps
    fv(ptt: %): ListSet Symbol == rep(ptt).fv

    varRef(sym: Symbol, fv: Symbol): % ==
        t: Expression := expr fv
        per [[fv], t, param(sym, constant t)]

    withType(ptt: %, tt: Expression): % ==
        per [fv ptt, tt, rep(ptt).ps]

    withParams(ptt: %, ps: ParamSet): % ==
        per [fv ptt, rep(ptt).tt, ps]

    withType(ptt: %, tt: TypeTerm): % ==
        per [fv ptt + vars tt, expr tt, rep(ptt).ps]

    asParam(tt: TypeTerm): % == per [vars tt, expr tt, empty()]

    sexpression(ptt: %): SExpression ==
        [sexpr(-"ptt"), [sexpr(s) for s in fv ptt], sexpression typeTerm ptt, sexpression paramSet ptt]

    (=)(a: %, b: %): Boolean ==
        stdout << "ptt eq " << a << " " << b << newline
        forall(fv a, rep(a).tt) = forall(fv b, rep(b).tt) and rep(a).ps = rep(b).ps
