#include "algebra"
#include "aldorio"

ListIterator(T: with): with {
    reset: % -> %;
    hasNext?: % -> Boolean;
    next: % -> %;
    current: % -> T;
    iterator: List T -> %;
    if T has OutputType then OutputType;
}
== add {
    Rep ==> Cross(l: List T, i: List T);
    import from Rep;
    import from List T;

    iterator(lst: List T): % == per cross(lst, lst);

    local cross(l: List T, i: List T): Cross(List T, List T) == (l, i);
    reset(iter: %): % == { (w, i) := rep(iter); per cross(w, w)}

    hasNext?(iter: %): Boolean == {
        (l, i) := rep iter;
        not empty? rest i;
    }

    next(iter: %): % == {
        (l, i) := rep iter;
        per cross(l, rest i);
    }

    current(iter: %): T == {
        (l, i) := rep iter;
        first i
    }

    if T has OutputType then (o: TextWriter) << (i: %): TextWriter == {
        import from T;
        (l, ll) := rep i;
        o << "{I: " << l << ", " << ll << "}";
    }
}

State(T: OutputType): with {
     init: List List T -> %;
     next: % -> (Partial %, List T);
     generator: % -> Generator List T;
}
== add {
    Rep ==> List ListIterator T;
    import from Rep;
    import from ListIterator T;

    init(initlst: List List T): % == per [iterator l for l in initlst];

    generator(iter: %): Generator List T == generate {
        import from Partial %;
        done := false;
        while not done repeat {
            (nextIter, nextElt) := next iter;
            yield nextElt;
            if failed? nextIter then done := true;
            else {
                iter := retract nextIter;
                yield nextElt;
            }
        }
    }


    next(state: %): (Partial %, List T) == {
        ll := rep state;
        nextElt: List T := [];
        nextState: List ListIterator T := [];
        carry: Boolean := true;
        while carry and not empty? ll repeat {
            iter := first ll;
            nextElt := cons(current iter, nextElt);
            if hasNext? iter then {
                nextIter := next iter;
                nextState := cons(nextIter, nextState);
                carry := false;
            }
            else {
                iter := reset iter;
                nextState := cons(iter, nextState);
                carry := true;
            }
            ll := rest ll;
        }
        while (not empty? ll) repeat {
            iter := first ll;
            nextElt := cons(current iter, nextElt);
            nextState := cons(iter, nextState);
            ll := rest ll;
        }
        carry => (failed, nextElt);
        ([per reverse! nextState], nextElt);
    }
}

Dnf: BooleanAlgebra with {
    normalForm: LogicExpression -> %;
    evaluate: (T: BooleanAlgebra, IndexedAtom -> T) -> (% -> T);
}
== add {
    Rep == List AndClause;
    import from Rep;
    import from AndClause;
    import from LogicExpression;
    default dnf, dnf1, dnf2: %;

    false: % == per [];
    true: % == per [true()];

    _not dnf: % == error "Not implemented";
    _and(dnf1, dnf2): % == error "Not implemented";
    _or(dnf1, dnf2): % == error "Not implemented";

    true? dnf: Boolean == dnf = true;
    false? dnf: Boolean == empty? rep dnf;

    dnf1 = dnf2: Boolean == rep(dnf1) = rep(dnf2);

    extree dnf: ExpressionTree == {
        import from ListMapper(AndClause, ExpressionTree);
        import from ExpressionTreeLeaf;
        empty? rep dnf => extree leaf true;
        ExpressionTreeList map(extree) rep dnf;
    }

    normalForm(inexpr: LogicExpression): % == {
        e := removeNegations(inexpr);
        e := distribution(e);
        dnf := dnfFromNormalForm e;
        dnf
    }

    dnfFromNormalForm(e: LogicExpression): % == {
        import from List LogicExpression;
        clause? e => per [clause e];
        or? e => per [clause term for term in terms e];
        stdout << "Not in normal form " << e << newline;
        never;
    }

    local removeNegations(e: LogicExpression): LogicExpression == {
        import from List LogicExpression;
        import from List IndexedAtom;
        import from IndexedAtom;
        clause? e => e;
        empty? terms e => e;
        and? e => _and [removeNegations term for term in terms e];
        or? e => _or [removeNegations term for term in terms e];
        i := _not e;
        not? i => removeNegations(_not i);
        clause? i and empty? rest atoms i => expression negate first atoms i;
        or? i => _and([removeNegations(_not ie) for ie in terms i]);
        and? i => _or([removeNegations(_not ie) for ie in terms i]);
        error "Negations case";
    }

    distribution(e: LogicExpression): LogicExpression == {
        import from List LogicExpression;
        import from LogicExpression;
        import from Fold2(LogicExpression, List LogicExpression);
        clause? e => e;
        false? e => e;
        true? e => e;
        not? e => error "No nots allowed";
        and? e => {
            import from List List LogicExpression;
            import from State LogicExpression;
            qq := [distribution clause for clause in terms e];
            l: List List LogicExpression := [ [atom for atom in terms subexpr] for subexpr in qq];
            _or[_and(clause) for clause in init l];
        }
        or? e => {
            ll := [distribution term for term in terms e];
            _or ll;
        }
        return e;
    }


    distribution0(e: LogicExpression): LogicExpression == {
        import from List LogicExpression;
        not? e => {
            stdout << "Remove negation failed " << newline;
            never;
        }
        clause? e => e;
        not or? e => e;-- WRONG!
        eT := terms e;
        empty? eT => e;
        empty? rest eT => never;
        t0 := first eT;
        r := rest eT;
        and? t0 => {
            -- or(and(t00, t0R), r) ==> and(or(t00, r), or(t0R, r))
            t00 := first terms t0;
            t0R := rest terms t0;
            _and(distribution(_or(t00, _and(r))), distribution(_or(_and(t0R), _and(r))));
        }
        _or(distribution t0, distribution(_or(r)));
    }

    evaluate(T: BooleanAlgebra, atomToT: IndexedAtom -> T)(v: %): T == {
        import from T;
        import from List OrClause;
        import from List IndexedAtom;
        import from Fold2(T, T);
        import from ListMapper(IndexedAtom, T);
        (_or, false)/((_and, true)/map(atomToT) atoms clause for clause in rep(v));
    }

}

#if ALDORTEST
#include "algebra"
#include "aldorio"
#pile

test(): () ==
    import from LogicExpression;
    import from Integer;
    import from IndexedAtom;
    l: LogicExpression := expression atom 1;
    dnf: Dnf := normalForm l;
    stdout << l << " --> " << dnf << newline;
    l := _or(expression atom 1, expression atom 2);
    dnf := normalForm l;
    stdout << l << " --> " << dnf << newline;
    l := _and(expression atom 1, expression atom 2);
    dnf := normalForm l;
    stdout << l << " --> " << dnf << newline;
    l := _or(l, expression atom 3);
    dnf := normalForm l;
    stdout << l << " --> " << dnf << newline;

    l := _or(_not l, expression atom 3);
    dnf := normalForm l;
    stdout << l << " --> " << dnf << newline;

    l := _and(_or(expression atom 1, expression atom 2), expression atom 3);
    dnf := normalForm l;
    stdout << l << " --> " << dnf << newline;




allExpressions(depth: Integer, nAtoms: Integer): HashSet LogicExpression == {
    import from List SortedSet LogicExpression;
    import from List LogicExpression;
    import from LogicExpression;
    import from IndexedAtom;
    if depth = 0 then [expression atom n for n in 1..nAtoms]
    else {
        exprs: HashSet LogicExpression := allExpressions(depth - 1, nAtoms);
        for e in exprs repeat
            stdout << e << newline;
        hs := copy exprs;
        for e in exprs | not not? e repeat insert!(hs, _not e);
        for e1 in exprs repeat {
            for e2 in exprs repeat {
                insert!(hs, _and(e1, e2));
                insert!(hs, _or(e1, e2));
            }
        }
        hs
    }
}

--test();

foo(): () ==
    import from Integer;
    import from HashSet LogicExpression;
    import from LogicExpression;
    for d in 1..3 repeat for n in 1..3 repeat stdout << d << " " << coerce(#allExpressions(d, n)) << newline;
    stdout << "hello " << newline;
    for e in allExpressions(1, 1) repeat stdout << e << newline;
    stdout << "hello2 " << newline;
    for e in allExpressions(1, 2) repeat stdout << e << newline;

--foo();


testEvaluate(): () ==
    import from LogicExpression
    import from HashSet LogicExpression
    import from List Bit, Bit, Assert Bit
    import from Dnf
    import from Integer
    import from MachineInteger
    import from IndexedAtom
    allExprs := allExpressions(2, 2);

    for expr in allExprs repeat
        dnf := normalForm expr;
        ll: List List Bit := [[false, false], [false, true], [true, false], [true, true]]
        valuation(i: MachineInteger)(atom: IndexedAtom): Bit ==
            v := ll.(i).(machine index atom);
            if negated? atom then _not v else v;
        for i in 1..4@MachineInteger repeat
            assertEquals(evaluate(Bit, valuation(i)) expr,
                         evaluate(Bit, valuation(i)) dnf);

testEvaluate()



iterateState(): () ==
    import from State Integer;
    import from List Integer, List List Integer;
    import from Partial State Integer;
    import from Integer;
    ll := [[1,2], [10]]
    state := init ll;
    done := false;
    while not done repeat
        (nextState, elt) := next state;
        if failed? nextState then done := true;
        else state := retract nextState;

iterateState()

#endif
