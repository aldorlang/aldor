#include "types"
#include "aldorio"
#pile

XInferState: with
    init: (Unit Expression, XEnv) -> %

    env: % -> XEnv
    base: % -> Expression
    pushEnv!: (%, Expression) -> ()
    popEnv!: (%, Expression) -> ()

    annotate!: (%, Expression, XTPoss) -> ()
    annotation: (%, Expression) -> XTPoss
    annotations: % -> Annotated(Expression, XTPoss)

    type!: (%, Expression, TypeTerm) -> ()
    type:  (%, Expression) -> TypeTerm
    types: % -> Annotated(Expression, TypeTerm)

    envPhase: NamedAnnotation(Expression, XEnv)
    bupPhase: NamedAnnotation(Expression, XTPoss)
    tdnPhase: NamedAnnotation(Expression, TypeTerm)

    initBup!: % -> ()
    initTdn!: % -> ()
== add
    Rep == Record(stack: List XEnv, u: Unit Expression)

    import from Annotated(Expression, XEnv)
    import from Annotated(Expression, XTPoss)
    import from Annotated(Expression, TypeTerm)
    import from List XEnv
    import from Unit Expression
    import from Rep

    init(u: Unit Expression, curr: XEnv): % == per [[curr], u]

    base(state: %): Expression == base rep(state).u

    initBup!(state: %): () ==
        tbl: Annotated(Expression, XTPoss) := new()
        register!(rep(state).u, XTPoss, bupPhase, tbl)

    initTdn!(state: %): () ==
        tbl: Annotated(Expression, TypeTerm) := new()
        register!(rep(state).u, TypeTerm, tdnPhase, tbl)

    pushEnv!(state: %, e: Expression): () ==
        if (annotation?(bindings state, e)) then
            rep(state).stack := cons(annotation(bindings state, e), rep(state).stack)

    popEnv!(state: %, e: Expression): () ==
        if (annotation?(bindings state, e)) then
            rep(state).stack := rest(rep(state).stack)

    env(state: %): XEnv == first rep(state).stack
    bindings(state: %): Annotated(Expression, XEnv) == annotations(rep(state).u, XEnv, envPhase)

    envPhase: NamedAnnotation(Expression, XEnv) == new "bind"
    bupPhase: NamedAnnotation(Expression, XTPoss) == new "bup"
    tdnPhase: NamedAnnotation(Expression, TypeTerm) == new "tdn"

    annotations(state: %): Annotated(Expression, XTPoss) == annotations(rep(state).u, XTPoss, bupPhase)
    annotate!(state: %, expr: Expression, tp: XTPoss): () == annotate!(annotations(state), expr, tp)
    annotation(state: %, e: Expression): XTPoss == annotation(annotations state, e)

    types(state: %): Annotated(Expression, TypeTerm) == annotations(rep(state).u, TypeTerm, tdnPhase)
    type(state: %, e: Expression): TypeTerm == annotation(types state, e)
    type!(state: %, e: Expression, tt: TypeTerm): () == annotate!(types state, e, tt)

