#include "types"
#include "aldorio"
#pile
	
XScopeBind: with
    bind: (Unit Expression, e: XEnv) -> ()
    annotations: Unit Expression -> Annotated(Expression, XEnv)
== add
    import from List Expression
    import from ListSet Symbol
    import from Expression
    import from TypeTerm
    import from Symbol
    import from SymbolMeaningFields
    import from TermFields
    import from XTypes
    
    phase: NamedAnnotation(Expression, XEnv) == new "bind"
    annotations(u: Unit Expression): Annotated(Expression, XEnv) ==
        annotations(u, XEnv, phase)

    bind(u: Unit Expression, env: XEnv): () ==
        tbl: Annotated(Expression, XEnv) := new()
	bindInner(ee: XEnv, e: Expression): () ==
	    lambda? e =>
	        newEnv := push(ee)
	        newEnv2 := push(newEnv)
		populateLambdaLevel(newEnv, e)
	        annotate!(tbl, e, newEnv)
	        annotate!(tbl, lambdaBody e, newEnv2)
		bindInner(newEnv, lambdaBody(e))
	    def? e =>
	       bindDefine(e)	       
	    for p in parts e repeat
	        bindInner(ee, p)
 	bindDefine(e: Expression): () ==
            var := defLhs(e)
            rhs := defRhs(e)
	    declare? var =>
	        put!(env, declareVar var, constant declareType var)
	    term? var =>
	        varType := newVar()
	        stdout << "Type of defined " << var << " " << varType << newline
	        put!(env, term var, varType)
	        bindInner(env, rhs)
	    error("Unknown var")
        annotate!(tbl, base u, env)
        bindInner(env, base u)
	register!(u, XEnv, phase, tbl)

    local populateLambdaLevel(env: XEnv, lexpr: Expression): () ==
        for var in lambdaVars lexpr repeat
	    declare? var =>
	        put!(env, declareVar var, constant declareType var)
	    term? var =>
	        varType := newVar()
		stdout << "Type of " << var << " " << varType << newline
		put!(env, term var, varType)
	    error "Unknown var"
