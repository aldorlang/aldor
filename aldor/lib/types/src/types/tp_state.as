#include "types"
#include "aldorio"
#pile

InferState: with
    init: (Unit Expression, Env) -> %

    env: % -> Env
    base: % -> Expression
    pushEnv!: (%, Expression) -> ()
    popEnv!: (%, Expression) -> ()

    annotate!: (%, Expression, TPoss) -> ()
    annotation: (%, Expression) -> TPoss
    annotations: % -> Annotated(Expression, TPoss)

    type!: (%, Expression, TypeTerm) -> ()
    type:  (%, Expression) -> TypeTerm
    types: % -> Annotated(Expression, TypeTerm)

    envPhase: NamedAnnotation(Expression, Env)
    bupPhase: NamedAnnotation(Expression, TPoss)
    tdnPhase: NamedAnnotation(Expression, TypeTerm)

    initBup!: % -> ()
    initTdn!: % -> ()
== add
    Rep == Record(stack: List Env, u: Unit Expression)

    import from Annotated(Expression, Env)
    import from Annotated(Expression, TPoss)
    import from Annotated(Expression, TypeTerm)
    import from List Env
    import from Unit Expression
    import from Rep

    init(u: Unit Expression, curr: Env): % == per [[curr], u]

    base(state: %): Expression == base rep(state).u

    initBup!(state: %): () ==
        tbl: Annotated(Expression, TPoss) := new()
        register!(rep(state).u, TPoss, bupPhase, tbl)

    initTdn!(state: %): () ==
        tbl: Annotated(Expression, TypeTerm) := new()
        register!(rep(state).u, TypeTerm, tdnPhase, tbl)

    pushEnv!(state: %, e: Expression): () ==
        if (annotation?(bindings state, e)) then
            rep(state).stack := cons(annotation(bindings state, e), rep(state).stack)

    popEnv!(state: %, e: Expression): () ==
        if (annotation?(bindings state, e)) then
            rep(state).stack := rest(rep(state).stack)

    env(state: %): Env == first rep(state).stack
    bindings(state: %): Annotated(Expression, Env) == annotations(rep(state).u, Env, envPhase)

    envPhase: NamedAnnotation(Expression, Env) == new "bind"
    bupPhase: NamedAnnotation(Expression, TPoss) == new "bup"
    tdnPhase: NamedAnnotation(Expression, TypeTerm) == new "tdn"

    annotations(state: %): Annotated(Expression, TPoss) == annotations(rep(state).u, TPoss, bupPhase)
    annotate!(state: %, expr: Expression, tp: TPoss): () == annotate!(annotations(state), expr, tp)
    annotation(state: %, e: Expression): TPoss == annotation(annotations state, e)

    types(state: %): Annotated(Expression, TypeTerm) == annotations(rep(state).u, TypeTerm, tdnPhase)
    type(state: %, e: Expression): TypeTerm == annotation(types state, e)
    type!(state: %, e: Expression, tt: TypeTerm): () == annotate!(types state, e, tt)

