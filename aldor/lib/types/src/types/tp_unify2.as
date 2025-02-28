#include "types"
#include "aldorio"
#pile

Unifier: SExpressionOutputType with
    fv: % -> ListSet Symbol
    sigma: % -> Substitution
    params: % -> ParamSet
    failed: () -> %
    failed?: % -> Boolean

    unifier: (ListSet Symbol, Substitution, ParamSet) -> %

    apply: (%, ParamSet) -> ParamSet
    apply: (%, TypeTerm) -> TypeTerm
    apply: (%, ParamTypeTerm) -> ParamTypeTerm

    addSubst: (%, Symbol, Expression) -> %
    addParam: (%, Expression) -> %
    addFv: (%, Symbol) -> %

    empty: () -> %
    empty?: % -> Boolean

    fv?: (Symbol, %) -> Boolean
== add
    Rep == Record(failed: Boolean, fvars: ListSet Symbol, sigma: Substitution, params: ParamSet)
    import from Rep
    import from Substitution, ParamSet, ListSet Symbol
    import from Symbol
    import from TypeTerm
    
    local theFailed: % == per [true, [], empty(), []]

    unifier(fv: ListSet Symbol, sigma: Substitution, ps: ParamSet): % ==
        per [false, fv, sigma, ps]

    fv(u: %): ListSet Symbol == rep(u).fvars
    sigma(u: %): Substitution == rep(u).sigma
    params(u: %): ParamSet == [forall(fv u, expr e) for e in rep(u).params]

    empty(): % == per [false, [], empty(), []]
    empty?(u: %): Boolean == not failed? u and empty? sigma u and empty? params u

    failed(): % == per [true, [], empty(), []]
    failed?(u: %): Boolean == rep(u).failed

    fv?(v: Symbol, u: %): Boolean == member?(v, fv u)

    addSubst(u: %, v: Symbol, e: Expression): % == per [false, fv u, addSubst(sigma u, v, e), params u]
    addParam(u: %, e: Expression): % == per [false, fv u, sigma u, params u + [constant e]]

    addFv(u: %, v: Symbol): % == per [false, fv u + [v], sigma u, params u]

    apply(u: %, ptt: ParamTypeTerm): ParamTypeTerm ==
        tt := forall(fv u + fv ptt - [sym for (sym, expr) in sigma u], expr u typeTerm ptt)
	pp := u paramSet ptt
	withParams(asParam tt, pp + params u)

    apply(u: %, p: ParamSet): ParamSet == [u tt for tt in p]

    apply(u: %, tt: TypeTerm): TypeTerm ==
        failed? u => error "Cannot apply a failed unifier"
        newExpr := (sigma u) expr tt
	ls: ListSet Symbol := []
	substVars: ListSet Symbol := [sym for (sym, expr) in sigma u]
	forall(fv u + vars tt - substVars, newExpr)

    sexpression(u: %): SExpression ==
        failed? u =>
	    [sexpr(-"unifier"), sexpr(-"failed")]
        [sexpr(-"unifier"), [sexpr x for x in fv u], sexpression sigma u, sexpression params u]

UnifierPair: SExpressionOutputType with
    fv: % -> ListSet Symbol
    swap: % -> %

    failed: () -> %
    empty: () -> %
    empty?: % -> Boolean
    failed?: % -> Boolean
    unified?: % -> Boolean

    left: % -> Unifier
    right: % -> Unifier
    unifier: % -> Unifier
    
    pair: (Unifier, ParamSet, ParamSet) -> %
    withParams: (UnifierPair, ParamSet, ParamSet) -> %
    withUnifier: (UnifierPair, Unifier) -> %

    lParam: % -> ParamSet
    rParam: % -> ParamSet
== add
    Rep == Record(fail: Boolean, u: Unifier, lParam: ParamSet, rParam: ParamSet)
    import from Rep
    import from ListSet Symbol
    import from ParamSet
    import from Substitution
    import from Symbol
        
    failed(): % == per [true, failed(), empty(), empty()]

    fv(ur: %): ListSet Symbol == fv unifier ur
    
    unifier(ur: %): Unifier == rep(ur).u

    left(ur: %): Unifier ==
        failed? ur => failed()
        unifier(fv ur, sigma rep(ur).u, rep(ur).lParam)

    right(ur: %): Unifier ==
        failed? ur => failed()
        unifier(fv ur, sigma rep(ur).u, rep(ur).rParam)

    lParam(ur: %): ParamSet == rep(ur).lParam
    rParam(ur: %): ParamSet == rep(ur).rParam

    pair(u: Unifier, l: ParamSet, r: ParamSet): % == per [failed? u, u, l, r]	

    withParams(up: UnifierPair, l: ParamSet, r: ParamSet): % == per [failed? up, unifier up, l, r]
    withUnifier(up: UnifierPair, u: Unifier): % == per [failed? u, u, lParam up, rParam up]

    swap(p: %): % == per [failed? p, unifier p, rep(p).rParam, rep(p).lParam]
    failed?(p: %): Boolean == rep(p).fail
    unified?(p: %): Boolean == not failed? p

    empty?(p: %): Boolean == empty? left p and empty? right p

    empty(): % == per [false, empty(), empty(), empty()]

    sexpression(ur: %): SExpression ==
        failed? ur => [sexpr(-"ur"), sexpr(-"failed")]
	[sexpr(-"ur"), sexpression unifier ur, sexpression rep(ur).lParam, sexpression rep(ur).rParam]

UnificationTools2: with
    unify: (ptt1: ParamTypeTerm, ptt2: ParamTypeTerm) -> UnifierPair
    unify: (tt1: TypeTerm, tt2: TypeTerm) -> UnifierPair
    unify: (tt1: ParamSet, tt2: ParamSet) -> UnifierPair
    merge: (UnifierPair, UnifierPair) -> UnifierPair
    merge: (Unifier, Unifier) -> Unifier
    mergeParams: (Unifier, Unifier) -> Unifier

    export from UnifierPair
== add
    import from Expression
    import from List Expression
    import from ListSet Symbol
    import from MachineInteger
    import from ParamTypeTerm
    import from ParamSet
    import from Partial Expression
    import from Partial TypeTerm
    import from Substitution
    import from TypeTerm
    import from Symbol
    import from Unifier
    import from UnifierPair
    
    unifyCounter: CallCounter := counter("unify", false)
    unifyTypeCounter: CallCounter := counter("unifyType", false)
    unifyParamCounter: CallCounter := counter("unifyParam", false)

    unify(ps1: ParamSet, ps2: ParamSet): UnifierPair == unifyParams(empty(), ps1, ps2)

    unify(tt1: TypeTerm, tt2: TypeTerm): UnifierPair == unify(asParam tt1, asParam tt2)

    unify(ptt1: ParamTypeTerm, ptt2: ParamTypeTerm): UnifierPair ==
        count := open(unifyCounter)
        writer(count) << "(Unify " << count << ": " << ptt1 << " <--> " << ptt2 << newline
        result: UnifierPair := unify1(ptt1, ptt2)
        writer(count) << " Unify " << count << ": " << ptt1 << " <--> " << ptt2 << " ===> " << result << ")" << newline
	validateResult(result, ptt1, ptt2)
        validate(result)
	close(count)
        result

    local validateResult(pair: UnifierPair, ptt1: ParamTypeTerm, ptt2: ParamTypeTerm): () ==
        failed? pair => true
        expr typeTerm apply(left pair, ptt1) ~= expr typeTerm apply(right pair, ptt2) =>
	    stdout << "Failed 1 - " << apply(left pair, ptt1) << newline
	    stdout << "Failed 2 - " << apply(right pair, ptt2) << newline
	    never
	true
	
    local validate(pair: UnifierPair): () ==
        validate left pair
	validate right pair

    local validate(u: Unifier): () ==
        import from List Cross(Symbol, Expression)
        issues: List String := []
	substVars: ListSet Symbol := [v for (v, e) in substs sigma u]
	if #substVars ~= # substs sigma u then issues := cons("repeated substs", issues)
        for (var, expr) in substs sigma u repeat
            if terms expr - substVars ~= terms expr then issues := cons("Substs on rhs ", issues)
	if not empty? issues then
	    for issue in issues repeat
	        stdout << "Error: " << issue << newline
		never

    local unify1(ptt1: ParamTypeTerm, ptt2: ParamTypeTerm): UnifierPair == 
        u0: UnifierPair := empty()
	u0 := unifyOne(u0, typeTerm ptt1, typeTerm ptt2)
	u0 := unifyParams(u0, paramSet ptt1, paramSet ptt2)
	return u0
        -- not sure what this does	
	-- unifyVars(u0, vars(typeTerm ptt1), vars(typeTerm ptt2)) 
#if 0
    local unifyVars(u: UnifierPair, v1: ListSet Symbol, v2: ListSet Symbol): UnifierPair ==
        failed? u => u
        cc := current(mergeCounter)
	-- add dummy substitutions for any vars not substituted
	nv: List Symbol := [v for v in v1 + v2 | not var?(v, sigma unifier u)]
	empty? nv => u
	u0 := unifier u
	for v in nv repeat
            lineWriter(cc) << "Adding var " << v1 << " " << v2 << " Unifier " << u << newline
	    u0 := addFv(u0, v)
	return withUnifier(u, u0)

#endif
    local unifyParams(u0: UnifierPair, ps1: ParamSet, ps2: ParamSet): UnifierPair ==
        failed? u0 => return failed()
	
        import from Partial TypeTerm
        local lhsSplit: List Cross(TypeTerm, Partial TypeTerm)
        local rhsSplit: List Cross(TypeTerm, Partial TypeTerm)
	writer(unifyParamCounter) << "Unify params - orig: " << u0 << newline
	writer(unifyParamCounter) << "Unify params - ps1: " << ps1 << newline
	writer(unifyParamCounter) << "Unify params - ps2: " << ps2 << newline

	lhsSplit := [(p, find(paramVar expr p, ps2)) for p in ps1]
	rhsSplit := [(p, find(paramVar expr p, ps1)) for p in ps2]
	lhs: ParamSet := [p1 for (p1, pp2) in lhsSplit | not pp2]
        rhs: ParamSet := [p2 for (p2, pp1) in rhsSplit | not pp1]

 	writer(unifyParamCounter) << "- LHS: " << lhs << newline
 	writer(unifyParamCounter) << "- RHS: " << rhs << newline

        for (p1, pp2) in lhsSplit repeat
	    failed? pp2 => iterate
	    writer(unifyParamCounter) << "  - ParamUnify " << p1 << " <--> " << pp2 << newline
	    u0 := unifyOne(u0, p1, retract pp2)
            writer(unifyParamCounter) << "  - ParamUnify: ==> " << u0 << newline
	    failed? u0 => return failed()
	extraLeft: ParamSet := [apply(left u0, lp) for lp in lhs]
	extraRight: ParamSet := [apply(right u0, rp) for rp in rhs]

 	writer(unifyParamCounter) << "- ExtraLeft:  " << extraLeft << newline
 	writer(unifyParamCounter) << "- ExtraRight: " << extraRight << newline

        writer(unifyParamCounter) << "Unified params " << u0 << " L: " << extraLeft << " R: " << extraRight << newline
	return withParams(u0, extraRight, extraLeft)

    unifyOne(u0: UnifierPair, tt1: TypeTerm, tt2: TypeTerm): UnifierPair ==
        failed? u0 => u0
	ur := unifyTypes(tt1, tt2)
	m := merge(u0, ur)
	writer(unifyCounter) << "Unify " << u0 << " + " << tt1 << " <--> " << tt2 << newline
	writer(unifyCounter) << "Unify ==> " << ur << newline
	writer(unifyCounter) << "UnifyMerger ==> " << m << newline
	return m

    unifyTypes(tt1: TypeTerm, tt2: TypeTerm): UnifierPair ==
        count := open(unifyTypeCounter)
        writer(count) << "(UnifyType " << count << ": " << tt1 << " <--> " << tt2 << newline
        result := unifyTypes1(tt1, tt2)
        writer(count) << " UnifyType " << count << ": " << tt1 << " <--> " << tt2 << " ===> " << result << ")" << newline
        validate(result)
	close(count)
        result

    -- this could work on Unifier
    local unifyTypes1(tt1: TypeTerm, tt2: TypeTerm): UnifierPair ==
        term? expr tt2 and not term? expr tt1 => swap unifyTypes(tt2, tt1)
        term? expr tt1 and term? expr tt2 =>
	    var?(tt1, term expr tt1) and var?(tt2, term expr tt2) =>
	        newVar: Symbol := new()
		subst := sigma( (term expr tt1, expr newVar), (term expr tt2, expr newVar))
		--pair([newVar], sigma(term expr tt1, expr newVar), sigma(term expr tt2, expr newVar))
		newU: Unifier := unifier([newVar], sigma( (term expr tt1, expr newVar), (term expr tt2, expr newVar)), empty())
		pair(newU, empty(), empty())
            var?(tt1, term expr tt1) =>
	        --pair([], sigma(term expr tt1, expr tt2), empty())
		newU: Unifier := unifier([], sigma(term expr tt1, expr tt2), empty()) --NB: Vars?
		pair(newU, empty(), empty())
	    var?(tt2, term expr tt2) => 
		newU: Unifier := unifier([], sigma(term expr tt2, expr tt1), empty()) -- NB: Vars?
		pair(newU, empty(), empty())
	    term expr tt1 = term expr tt2 =>
	        pair(empty(), empty(), empty())
	    failed()
	term? expr tt1 =>
	    var?(tt1, term expr tt1) =>
	        --pair(vars tt2, sigma(term expr tt1, expr tt2), empty())
		newU: Unifier := unifier(vars tt2, sigma(term expr tt1, expr tt2), empty())
		pair(newU, empty(), empty())
	    failed()
	application? expr tt1 and not application? expr tt2 =>
	    failed()
	application? expr tt1 =>
	    r1 := unifyTypes(part(tt1, applicationOp), part(tt2, applicationOp))
	    ll: List UnifierPair := []
	    r0 := r1
	    # applicationArgs expr tt1 ~= # applicationArgs expr tt2 => failed()
	    for tt1i in applicationArgs expr tt1 for tt2i in applicationArgs expr tt2 for i in 1.. repeat
	        writer(unifyTypeCounter) << "Unify arg " << i << newline
	        r1 := unifyOne(r1, forall(vars tt1, tt1i), forall(vars tt2, tt2i))
	        writer(unifyTypeCounter) << "Unify arg " << i << "complete" << newline
  	        ll := cons(r1, ll)
	    writer(unifyTypeCounter) << "UnifyApply - Start " << r0 << newline
	    for u in reverse ll for tti1 in applicationArgs expr tt1 for tti2 in applicationArgs expr tt2 repeat
	        writer(unifyTypeCounter)  << "Unify Apply--> " << tti1 << " <--> " << tti2 << " ==> " << u << newline
	    return r1
	comma? expr tt1 and not comma? expr tt2 => failed()
	comma? expr tt1 and comma? expr tt2 =>
	    # commaParts expr tt1 ~= # commaParts expr tt2 => failed()
	    r1 := empty()
	    for tfi1 in commaParts expr tt1 for tfi2 in commaParts expr tt2 repeat
	        rP := r1
	        r1 := unifyOne(r1, forall(vars tt1, tfi1), forall(vars tt2, tfi2))
		writer(unifyTypeCounter) << "UnifyComma " << rP << " --> " << r1 << newline
	    return r1
	param? expr tt1 and not param? expr tt2 => failed()
	param? expr tt1 and param? expr tt2 =>
	    paramVar expr tt1 = paramVar expr tt2 =>
	        return unifyTypes(part(tt1, paramExpr), part(tt2, paramExpr))
	    failed()
        writer(unifyTypeCounter) << "Missing case " << tt1 << " + " << tt2 << newline
        never

    mergePairCounter: CallCounter := counter("mergePair", false)

    merge(u1: UnifierPair, u2: UnifierPair): UnifierPair ==
        failed? u1 => failed()
	failed? u2 => failed()
	-- Something!
	cc := open(mergePairCounter)
	writer(cc) << "Merge " << unifier u1 << " <--> " << unifier u2 << newline
	writer(cc) << "Merge Left " << lParam u1 << " <--> " << lParam u2 << newline
	writer(cc) << "Merge Right " << rParam u1 << " <--> " << rParam u2 << newline
	uu: Unifier := merge(unifier u1, unifier u2)
	u: UnifierPair := pair(uu, empty(), empty())
	writer(cc) << "Merge done: " << cc << " " << u << newline
	close(cc)
	return u

    mergeCounter: CallCounter := counter("merge", true) 
    merge(u1: Unifier, u2: Unifier): Unifier ==
        cc := open(mergeCounter)
	writer(cc) << "(merge " << cc << ": " << u1 << " + " << u2 << newline
	r := merge1(u1, u2)
	writer(cc) << " merge " << cc << ": " << r << ")" << newline
	close(cc)
	return r

    -- Old version
    local merge0(u1: Unifier, u2: Unifier): Unifier ==
	cc := current(mergeCounter)
	failed? u1 => u1
	failed? u2 => u2
        empty? u1 and empty? fv u1 => u2
	empty? u2 and empty? fv u2 => u1
        final: Unifier := unifier(fv u1, sigma u1, empty())
	sigma2 := sigma u2
	while not empty? sigma2 repeat
	    (var, expr, rest) := split sigma2
	    sigma2 := rest
	    expr := (sigma final) expr
	    maybeOther: Partial Expression := lookup(sigma final, var)
	    writer(cc) << "Merge one " << cc << ": " << var << " -> " << expr << " + " << maybeOther << newline
	    if failed? maybeOther then
	        sig0: Substitution := sigma(var, expr)
		final := unifier(fv final - [var],
		                 addSubst(sigma( (var1, sig0 expr1) for (var1, expr1) in sigma final), var, expr),
		                 empty())
	    else
	        -- replace final with substituted and add (var, expr) as new
		other := retract maybeOther
		innerUnifyPair: UnifierPair := unifyTypes(forall(fv final, other), forall(fv final, expr))
		writer(cc) << "Merge one " << cc << ": " << var << " --> " << innerUnifyPair << newline
		newSigma := addSubst(sigma( (v, e) for (v, e) in sigma final | v ~= var), var, (sigma right innerUnifyPair) expr)
		writer(cc) << "Merge one " << cc << ": " << var << " Sigma " << newSigma << newline
		final := merge(unifier(fv final, newSigma, empty()), right innerUnifyPair)
	    writer(cc) << "Merge one " << cc << ": " << var << " => " << final << newline
	validate(final)
	final

    -- find sig0 from u1, restricted to where var is free in u2
    --   fixup params of v2 with sig0
    --   result is u2 with fv less the vars in sig0, sigma as applied and new params.
    -- Idea is that now, u2 has pulled in all the changes from u1 that are trivial
    local mergeFreeVars(u1: Unifier, u2: Unifier): Unifier ==
        cc := current(mergeCounter)
        sig0: Substitution := sigma( (v, e) for (v, e) in sigma u1 | fv?(v, u2))
	empty? sig0 => u2
	params2: ParamSet := [constant apply(sig0, expr tt) for tt in params u2]
	u2New := unifier(fv u2 - vars sigma u1, sigma( (v, sig0 e) for (v, e) in sigma u2), params2)
	writer(cc) << "MergeFree: " << u2 << " subst " << sig0 << newline
	writer(cc) << "MergeFree: " << u2New << newline
	return u2New
#if 0
    local merge1(u1: Unifier, u2: Unifier): Unifier ==
        cc := current(mergeCounter)
	failed? u1 => u1
	failed? u2 => u2
        empty? u1 and empty? fv u1 => u2 -- FIXME: Needs 'empty? fv'??
	empty? u2 and empty? fv u2 => u1 -- FIXME: Needs 'empty? fv'??
	u1 := mergeParams(u1, u2)
        u2 := mergeFreeVars(u1, u2)
        vars := fv u1 + fv u2
	addedVars := fv u1 + fv u2
	sigma2 := sigma u2
	params2 := params u2
	failed := false
	final := u1
	tmp := u2
        lineWriter(cc) << "(Merge: " << u1 << " + " << u2 << newline
        while not empty? sigma2 and not failed repeat
	    writer(cc) << "Merge step " << cc << ": " << final << " sigma2 " << sigma2 << newline
	    (var, expr, rest) := split sigma2
	    sigma2 := rest
	    maybeOther: Partial Expression := lookup(sigma u1, var)
	    if failed? maybeOther then
	        lineWriter(cc) << "Merge simple " << var << " = " << expr << newline   
	        lineWriter(cc) << "Merge simple - prev final " << final << newline
		final := substParam(final, var, expr)
	        final := addSubst(final, var, expr)
	        lineWriter(cc) << "Merge simple - sigma2 " << sigma2 << newline   
	        lineWriter(cc) << "Merge simple - final " << final << newline   
	        vars := vars - [var]
	    else
	        lExpr := retract maybeOther
	        uExpr := unifyTypes(forall(vars, lExpr), forall(vars, expr))
		xvars := [v for (v, e) in sigma unifier uExpr]
		newVars := fv uExpr
		vars := vars + newVars - [var] - xvars
		newSigma2: Substitution := empty()
		skipSubsts: Substitution := empty()
		for (v, e) in sigma2 repeat
		    if not member?(v, addedVars) then
		        newSigma2 := addSubst(newSigma2, v, (sigma unifier uExpr) e)
		    else
			lineWriter(cc) << "Dropped var " << v << newline
		        skipSubsts := addSubst(skipSubsts, v, e)
		for (v, e) in sigma unifier uExpr repeat
		    if not member?(v, addedVars) then
		        newSigma2 := addSubst(newSigma2, v, e)
		addedVars := addedVars + fv uExpr
		finalSigma := sigma((v, (sigma unifier uExpr) e) for (v, e) in sigma final)
		newFinal := unifier(vars, finalSigma, [constant apply(sigma unifier uExpr, expr(p)$TypeTerm) for p in params final])
		params2 := [constant apply(sigma unifier uExpr, expr(p)$TypeTerm) for p in params2]
		lineWriter(cc) << "Merge vars: " << var << " --> " << lExpr << " " << expr << newline
		lineWriter(cc) << "Merge vars: vars: " << vars << newline
		lineWriter(cc) << "Merge vars: Unifier: " << uExpr << newline
		lineWriter(cc) << "Merge vars: xvars: " << xvars << newline
		lineWriter(cc) << "Merge vars: newVars: " << newVars << newline
		lineWriter(cc) << "Merge vars: added: " << addedVars << newline
		lineWriter(cc) << "Merge vars: skipSubsts: " << skipSubsts << newline
		lineWriter(cc) << "Merge vars: newSigma2: " << newSigma2 << newline
		lineWriter(cc) << "Merge vars: newParams2: " << params2 << newline
		lineWriter(cc) << "Merge vars: newFinal: " << newFinal << newline
		sigma2 := newSigma2
		final := newFinal
        lineWriter(cc) << "Merge Result: " << u1 << " + " << u2 << newline
        lineWriter(cc) << "Merge Result: " << final << ")" << newline
	final
#endif
    local merge1(u1: Unifier, u2: Unifier): Unifier == never
#if 0    
        cc := current(mergeCounter)
	failed? u1 => u1
	failed? u2 => u2
	paramUnifier := unifyParams(empty(), params u1, params u2)
	stdout << "Param merge --> " << paramUnifier << newline
	params := apply(left paramUnifier, u1)
		
	sigma1 := compose(sigma u1, sigma left paramUnifier)	
	sigma2 := compose(sigma u2, sigma right paramUnifier)	
	
	fv1 := fv u1 - vars left paramUnifier + fv left paramUnifier
	fv2 := fv u2 - vars left paramUnifier + fv right paramUnifier

	merged := mergeSimple(fv1, sigma1, fv2, sigma2, params)

	return merged
#endif
    local mergeSimple(fvL: ListSet Symbol, sigmaL: Substitution,
                      fvR: ListSet Symbol, sigmaR: Substitution, params: ParamSet): Unifier == never
#if 0		      
        sigmaFinal: Substitution := empty()
	
	lVars := vars sigmaL
	rVars := vars sigmaR
	lSimpleSigma := [(v, e) for (v, e) in sigmaL | not contains?(rVars, e)]
	rSimpleSigma := [(v, e) for (v, e) in sigmaR | not contains?(lVars, e)]
	
	lSigma := compose(rSimpleSigma, sigmaL)
	rSigma := compose(lSimpleSigma, sigmaR)
	final := compose(lSigma, rSigma)
	mixSigma := empty()
	while not empty? lSigma repeat
	    (var, expr, rest) := split lSigma
	    maybeOther := lookup(rSigma, var)
	    failed? maybeOther => error "Should be common"
	    unified := unifyTypes(vars, expr, retract maybeOther)
	    mixSigma := compose(sigma unified, mixSigma)
	return final
#endif	
    local substParam(u: Unifier, sym: Symbol, e: Expression): Unifier ==
        s0 := sigma(sym, e)
        newParams: ParamSet := [constant apply(s0, expr p) for p in params u]
	unifier(fv u, sigma u, newParams)

    mergeParams(u1: Unifier, u2: Unifier): Unifier ==
        empty? params u2 => u1
        cc := current(mergeCounter)
	lineWriter(cc) << "(MergeParams " << u1 << " + " << u2 << newline
	final := u1
	params2 := params u2
        vars := fv final + fv u2
    	for p2 in params2 repeat
	    pterm := find(paramVar(expr(p2)$TypeTerm), params final)
	    if not pterm then
	        final := addParam(final, expr(p2)$TypeTerm)
	    else
	        uExpr := unifyTypes(forall(vars, expr(retract pterm)$TypeTerm), forall(vars,expr(p2)$TypeTerm))
		lineWriter(cc) << " - param merge " << uExpr << newline
		final := addParam(final, (sigma unifier uExpr) (expr(p2)$TypeTerm))
		newVars := fv uExpr
		xvars := [v for (v, e) in sigma unifier uExpr]
		vars := vars + newVars - xvars
		final := unifier(vars, compose(sigma final, sigma unifier uExpr), params final)
	lineWriter(cc) << "MergeParams Result: " << u1 << " + " << u2 << newline
	lineWriter(cc) << "MergeParams Result --> " << final << " )" << newline
        final

#if 0
    L           + R
    (Vx a -> x) + (a -> int)
    Unifiers: (x -> int), ()
    Both L & R cannot contain 'a'

    L           + R
    (a -> int)  + (Vx a -> x) 
    Unifiers: (), (x -> int)
    Both L & R cannot contain 'a'

	
#endif

TestUnify2: with
    testUnify: () -> ()
== add
    import from Assert Expression
    import from Assert ListSet Symbol
    import from Assert Partial Expression
    import from Assert ParamTypeTerm
    import from Assert MachineInteger
    import from Assert Substitution
    import from Assert TypeTerm
    import from Expression
    import from ListSet Symbol
    import from MachineInteger
    import from ParamSet
    import from ParamTypeTerm
    import from Partial Expression
    import from Partial TypeTerm
    import from Substitution
    import from Symbol
    import from TypeTerm
    import from UnificationTools2
    import from Unifier
    import from UnifierPair

    testUnifyFew(): () ==
	testUnifyComma3()
	testUnifyComma4()
	testUnifyComma5()
	testUnifyApplyComma1()

    testUnify(): () ==
        testUnifySimple1()
        testUnifySimple2()
        testUnifySimple3()
        testUnifySimple4()
        testUnifySimple5()
	testUnifyApply1()
	testUnifyApply2()
	testUnifyParamTypeTerm()
	testUnifyParamTypeTerm2()
	testUnifyMerge()
	testUnifyMerge2()
	testUnifyMerge3()
	testUnifyMerge4()
	testUnifyMerge5()
	testUnifyMerge6()
	testUnifyComma1()
	testUnifyComma2()
	testUnifyComma3()
	testUnifyComma4()
	testUnifyComma5()
	testUnifyApplyComma1()

    local test(tt1: TypeTerm, tt2: TypeTerm): () == test("test-unknown", tt1, tt2)

    local test(s: String, tt1: TypeTerm, tt2: TypeTerm): () ==
        stdout << "Starting test " << s << " " << tt1 << " <--> " << tt2 << newline
        up: UnifierPair := unify(tt1, tt2)
        stdout << "**Finished test - unifier" << s << " ---> " << up << newline
        stdout << "**Finished test " << s << " ---> " << apply(left up, tt1) << newline
        stdout << "**Finished test " << s << " ---> " << apply(left up, tt2) << newline
	assertEquals(apply(left up, tt1), apply(right up, tt2))

    local test(ptt1: ParamTypeTerm, ptt2: ParamTypeTerm): () == test("test-ptt-unknown", ptt1, ptt2)

    local test(s: String, ptt1: ParamTypeTerm, ptt2: ParamTypeTerm): () ==
        up: UnifierPair := unify(ptt1, ptt2)
	stdout << "Test Unified: " << ptt1 << " <--> " << ptt2  << newline
	stdout << "Test Unified: " << up << newline
	stdout << "Test left: " << left up << " --> " << apply(left up, ptt1) << newline
	stdout << "Test right: " << right up << " --> " << apply(right up, ptt2) << newline
	assertEquals(apply(left up, ptt1), apply(right up, ptt2))

    testUnifySimple1(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (x) x)"
	e2: TypeTerm := fromString "(apply foo a)"
	r := unify(e1, e2)
	expectedSigma: Substitution := sigma(-"x", fromString "(apply foo a)")
	assertEquals(expectedSigma, sigma left r)
	assertTrue(empty? fv left r)
	assertTrue(empty? fv right r)

    testUnifySimple2(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (x) x)"
	e2: TypeTerm := fromString "(forall (k) k)"
	test(e1, e2)

    testUnifySimple3(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall () x)"
	e2: TypeTerm := fromString "(forall (k) k)"
	test(e1, e2)

    testUnifySimple4(): () ==
        import from Expression
	e1: TypeTerm := fromString "(forall (k) k)"
        e2: TypeTerm := fromString "(forall () x)"
	test(e1, e2)

    testUnifySimple5(): () ==
        import from Expression
	e1: TypeTerm := fromString "(forall (k) int)"
        e2: TypeTerm := fromString "(forall (x) x)"
	test("simple-test-5", e1, e2)

    testUnifyApply1(): () ==
        e1: TypeTerm := fromString "(forall (x) (apply foo x))"
	e2: TypeTerm := fromString "(forall (k) (apply foo k))"
	test(e1, e2)

    testUnifyApply2(): () ==
        e1: TypeTerm := fromString "(forall (f) (apply f x))"
	e2: TypeTerm := fromString "(forall (k) (apply foo k))"
	test(e1, e2)

    testUnifyMerge(): () ==
        r1: Unifier := unifier([-"a"], empty(), empty())
	r2: Unifier := unifier([], sigma(-"a", fromString "int"), empty())
	m := merge(r1, r2)
	assertEquals([-"a"], fv m)

    testUnifyMerge2(): () ==
        r1: Unifier := unifier([], sigma(-"a", fromString "string"), empty())
	r2: Unifier := unifier([], sigma(-"b", fromString "int"), empty())
	m := merge(r1, r2)
	assertEquals([], fv m)
	assertEquals([fromString "string"], lookup(sigma m, -"a"))
	assertEquals([fromString "int"], lookup(sigma m, -"b"))
	assertEquals(2, # sigma m)

    testUnifyMerge3(): () ==
        r1: Unifier := unifier([-"b"], sigma(-"a", fromString "b"), empty())
	r2: Unifier := unifier([], sigma(-"a", fromString "int"), empty())
	stdout << "Test merge 3" << newline
	m := merge(r1, r2)
	assertEquals([], fv m)
	assertEquals([fromString "int"], lookup(sigma m, -"a"))
	--assertEquals([fromString "int"], lookup(sigma m, -"b"))
	assertEquals(1, # sigma m)

	m := merge(r2, r1)
	assertEquals([], fv m)
	assertEquals([fromString "int"], lookup(sigma m, -"a"))
	--assertEquals([fromString "int"], lookup(sigma m, -"b"))
	assertEquals(1, # sigma m)
	stdout << "Test merge 3 - 2/2" << newline

    testUnifyMerge4(): () ==
        r1: Unifier := unifier([], sigma(-"b", fromString "int"), empty())
	r2: Unifier := unifier([-"x"], sigma( (-"a", fromString "x"), (-"b", fromString "x")), empty())
	stdout << "Test merge 4" << newline
	m := merge(r1, r2)
	assertEquals([], fv m)
	assertEquals([fromString "int"], lookup(sigma m, -"a"))
	assertEquals([fromString "int"], lookup(sigma m, -"b"))
	--assertEquals([fromString "int"], lookup(sigma m, -"x"))
	assertEquals(2, # sigma m)

    --Merge (unifier (%v82) ((subst %v77 %v82) (subst %v81 %v82)) (pset () ()))
    -- <--> (unifier (%v81) ((subst %v78 (apply list %v81))) (pset () ()))
    testUnifyMerge5(): () ==
        r1: Unifier := unifier([-"v82"], sigma( (-"v77", fromString("v82")), (-"v81", fromString "v82")), empty())
        r2: Unifier := unifier([-"v81"], sigma( (-"v78", fromString("(apply list v81)"))), empty())
	m := merge(r1, r2)
	stdout << "Merge 5 " << r1 << " <--> " << r2 << newline	
	stdout << "Merge 5 " << m << newline
	assertEquals([-"v82"], fv m)
	assertEquals([fromString "v82"], lookup(sigma m, -"v81"))
	assertEquals([fromString "v82"], lookup(sigma m, -"v77"))
	assertEquals([fromString "(apply list v82)"], lookup(sigma m, -"v78"))
  	-- [82] 77 -> 82, 78 -> list v82

    -- Merge: (unifier () ((subst %v11 bool)) (pset () ()))
    --      + (unifier (%v14) ((subst %v12 %v14) (subst %v13 %v14)) (pset () ()))
    testUnifyMerge6(): () ==
        r1: Unifier := unifier([], sigma( (-"v11", fromString("bool"))), empty())
        r2: Unifier := unifier([-"v14"], sigma( (-"v12", fromString("v14")), (-"v13", fromString("v14"))), empty())
	m := merge(r1, r2)
	stdout << "Merge 5 " << r1 << " <--> " << r2 << newline	
	stdout << "Merge 5 " << m << newline
	assertEquals([-"v14"], fv m)
	assertEquals([fromString "bool"], lookup(sigma m, -"v11"))
	assertEquals([fromString "v14"], lookup(sigma m, -"v13"))
	assertEquals([fromString "v14"], lookup(sigma m, -"v12"))
	assertEquals([fromString "(apply list v82)"], lookup(sigma m, -"v78"))

    testMergeParams(): () == 
        r1: Unifier := unifier([-"v82"], sigma( (-"v1", fromString("int"))), [fromString "(param v1 int)"])
        r2: Unifier := unifier([-"v81"], sigma( (-"v2", fromString("int"))), [fromString "(param v1 int)"])
	m := merge(r1, r2)
	assertEquals(fromString "int", retract find(-"v1", params m))

    testUnifyParamTypeTerm(): () ==
        ptt1: ParamTypeTerm := withParams(asParam fromString "(forall (k) k)", fromString "((param x k))")
        ptt2: ParamTypeTerm := asParam fromString "(forall () y)"
	test(ptt1, ptt2)
	test(ptt2, ptt1)

    testUnifyParamTypeTerm2(): () ==
        ptt1: ParamTypeTerm := withParams(asParam fromString "(forall (k) k)", fromString "((param x k))")
        ptt2: ParamTypeTerm := withParams(asParam fromString "(forall (k) y)", fromString "((param x k))")
	test(ptt1, ptt2)
	test(ptt2, ptt1)

    testUnifyComma1(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (a b) (comma a b))"
	e2: TypeTerm := fromString "(forall (c) (comma c c))"
	test(e1, e2)
	test(e2, e1)

    testUnifyComma2(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (a b c) (comma a b b c c))"
	e2: TypeTerm := fromString "(forall (x y)   (comma x x y y q))"
	test(e1, e2)
	test(e2, e1)

    testUnifyComma3(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (a b c) (comma a b b c c))"
	e2: TypeTerm := fromString "(forall (x y z) (comma x q y y z))"
	test(e1, e2)
	test(e2, e1)

    testUnifyComma4(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (a) (comma a a))"
	e2: TypeTerm := fromString "(forall (c) (comma int int))"
	test(e1, e2)
	test(e2, e1)

    testUnifyComma5(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (a) (comma a a))"
	e2: TypeTerm := fromString "(forall (b) (comma b b))"
	test(e1, e2)
	test(e2, e1)

    testUnifyApplyComma1(): () ==
        import from Expression
        e1: TypeTerm := fromString "(forall (a) (apply map (comma a a) int))"
	e2: TypeTerm := fromString "(forall (b) (apply map (comma b b) int))"
	test(e1, e2)
	test(e2, e1)

-- Merge: (unifier () ((subst %v66 int)) (pset () ((%v63 (param %v63 int)))))
--      + (unifier () ((subst %v65 int)) (pset () ((%v63 (param %v63 int)))))

#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestUnify2
testUnify()
stdout << "DONE!" << newline
#endif
