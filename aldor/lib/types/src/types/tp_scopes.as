#include "types"
#include "aldorio"
#pile
	
ScopeBind: with
    bind: (Unit Expression, e: Env) -> ()
    annotations: Unit Expression -> Annotated(Expression, Env)
== add
    import from List Expression
    import from ListSet Symbol
    import from Expression
    import from TypeTerm
    import from Symbol

    phase: NamedAnnotation(Expression, Env) == new "bind"
    annotations(u: Unit Expression): Annotated(Expression, Env) ==
        annotations(u, Env, phase)

    bind(u: Unit Expression, env: Env): () ==
        tbl: Annotated(Expression, Env) := new()
	bindInner(ee: Env, e: Expression): () ==
	    lambda? e =>
	        newEnv := push(ee)
	        newEnv2 := push(newEnv)
		populateLambdaLevel(newEnv, e)
	        annotate!(tbl, e, newEnv)
	        annotate!(tbl, lambdaBody e, newEnv2)
		bindInner(newEnv, lambdaBody(e))
	    for p in parts e repeat
	        bindInner(ee, p)
        annotate!(tbl, base u, env)
        bindInner(env, base u)
	register!(u, Env, phase, tbl)

    local populateLambdaLevel(env: Env, lexpr: Expression): () ==
        for var in lambdaVars lexpr repeat
	    declare? var =>
	        put!(env, declareVar var, constant declareType var)
	    term? var =>
		pvar := new()$Symbol
		fvar := new()$Symbol
	        stdout << "Adding param " << var << " type is " << pvar << " " << fvar << newline
                tt: TypeTerm := forall(fvar, param(pvar, expr fvar))
		stdout << "Type of " << var << " " << tt << newline
	        putParamType!(env, term var, pvar)
		put!(env, term var, tt)
	    error "Unknown var"
