#include "algebra"
#include "aldorio"

OrClause: ExpressionType with {
    clause: IndexedAtom -> %;
    _or: Tuple % -> Partial %;
    atom: % -> Partial IndexedAtom;
    terms: % -> List IndexedAtom;

    false: () -> %;
    false?: % -> Boolean;
}
== add {
    Rep ==> List IndexedAtom;
    import from Rep;

    clause(a: IndexedAtom): % == per [a];
    false(): % == per [];
    false?(cl: %): Boolean == empty? rep cl;
    
    atom(cl: %): Partial IndexedAtom == {
        false? cl => failed;
	not empty? rest rep cl => failed;
	[first rep cl]
    }

    terms(cl: %): List IndexedAtom == rep cl;

    local safelyCons(a: IndexedAtom, b: Partial List IndexedAtom): Partial List IndexedAtom ==
        if failed? b then failed else [cons(a, retract b)];

    local merge(a: Rep, b: Rep): Partial Rep == {
        import from IndexedAtom, Integer;
        empty? a => [b];
        empty? b => [a];
        aatom := first a;
        batom := first b;
        if aatom = batom then safelyCons(first a, merge(rest a, rest b))
        else if isNegation?(aatom, batom) then failed;
        else if index aatom > index batom then safelyCons(first b, merge(a, rest b))
        else safelyCons(first a, merge(rest a, b));
    }

    local or2(a: %, b: %): Partial % == {
        import from Partial Rep;
        pr := merge(rep a, rep b);
        failed? pr => failed;
        [per retract pr];
    }

    _or(t: Tuple %): Partial % == {
       import from MachineInteger;
       acc := element(t, 1);
       for idx in 2..(length(t)) repeat {
           term := element(t, idx);
	   pacc := or2(acc, term);
	   failed? pacc => return failed;
	   acc := retract pacc;
       }
       [acc]
    }

    (a: %) = (b: %): Boolean == rep a = rep b;

    extree(a: %): ExpressionTree == {
        import from List ExpressionTree, IndexedAtom;
        import from ListMapper(IndexedAtom, ExpressionTree);
        import from ExpressionTreeLeaf;
        empty? rep a => extree leaf(true);
        ExpressionTreeList(map(extree)(rep a));
    }

}

LogicExpression: BooleanAlgebra with {
    expression: IndexedAtom -> %;
    not?: % -> Boolean;
    and?: % -> Boolean;
    or?: % -> Boolean;

    true?: % -> Boolean;
    false?: % -> Boolean;
}
== add {
    Rep ==> Union(AND: List %, OR: List %, NOT: %, CL: OrClause);
    import from Rep;
    import from OrClause;
    import from List %;
    default expr: %;
    
    not? expr: Boolean == rep(expr) case NOT;
    and? expr: Boolean == rep(expr) case AND;
    or? expr: Boolean == rep(expr) case OR or rep(expr) case CL;
    local clause? expr: Boolean == rep(expr) case CL;
    expression(a: IndexedAtom): % == per [clause a];

    local terms expr: List % == {
        import from List IndexedAtom;
        clause? expr => [expression atom for atom in terms rep(expr).CL];
	and? expr => rep(expr).AND;
	or? expr => rep(expr).OR;
	not? expr => [rep(expr).NOT];
	never;
    }

    true: % == per [ [], AND];
    false: % == per [ false(), CL];

    true? expr: Boolean == and? expr and empty? rep(expr).AND;
    false? expr: Boolean == clause? expr and false? rep(expr).CL;

    _not(e: %): % == {
    	true? e => false;
	false? e => true;
        not? e => rep(e).NOT;
	per [e, NOT];
    }

    _and(expr1: %, expr2: %): % == {
        and? expr1 and and? expr2 => per [append!(copy rep(expr1).AND, rep(expr2).AND), AND];
	per [[expr1, expr2], AND];
    }
    
    _or(expr1: %, expr2: %): % == {
        import from Partial OrClause;
        clause? expr1 and clause? expr2 => {
	    tmp := _or(rep(expr1).CL, rep(expr2).CL);
	    failed? tmp => true;
	    per [retract tmp, CL]
	}
        or? expr1 and or? expr2 => per [append!(copy terms expr1, terms expr2), OR];
	per [[expr1, expr2], OR];
    }

    extree(a: %): ExpressionTree == {
        import from List ExpressionTree;
        import from ExpressionTreeLeaf;
        import from ListMapper(LogicExpression, ExpressionTree);
        clause? a => extree(rep(a).CL);
        or? a => ExpressionTreePlus(map(extree) rep(a).OR);
        and? a => {
            empty? rep(a).AND => extree leaf false;
            ExpressionTreeTimes(map(extree) rep(a).AND);
        }
        not? a => ExpressionTreeMinus([extree(rep(a).NOT)]);
        never;
    }

    (expr1: %) = (expr2: %): Boolean == {
        and? expr1 => and? expr2 and rep(expr1).AND = rep(expr2).AND;
	clause? expr1 => clause? expr2 and rep(expr1).CL = rep(expr2).CL;
	or? expr1 => or? expr2 and rep(expr1).OR = rep(expr2).OR;
	not? expr1 => not? expr2 and rep(expr1).OR = rep(expr2).OR;
	false
    }
    
}

#if ALDORTEST
#include "algebra"
#include "aldorio"
#pile

test(): () == 
    import from OrClause
    import from Assert OrClause
    import from Assert Partial OrClause
    import from Partial OrClause
    import from IndexedAtom
    import from Integer
    atom1 := atom 1
    atom2 := atom 2

    assertEquals(false(), false())
    assertEquals(clause atom1, clause atom1)
    assertNotEquals(clause atom1, clause atom2)
    assertNotEquals(clause atom1, false())

    assertEquals([clause atom1], _or(false(), clause atom1))
    assertEquals([clause atom1], _or(clause atom1, clause atom1))
    assertEquals(_or(clause atom1, clause atom2), _or(clause atom2, clause atom1))
    assertEquals(_or(clause atom1, clause atom2, clause negate atom1), failed)

test2(): () ==
    import from Assert LogicExpression
    import from LogicExpression
    import from OrClause
    import from IndexedAtom
    import from Integer
    
    e1 := expression atom 1;
    e2 := expression atom 2;
    e3 := expression atom 3;

    assertTrue(or? _or(e1, e2))
    assertTrue(and? _and(e1, e2))
    assertTrue(and? _and(e1, _or(e1, e3)))
    assertTrue(not? _not(_or(e1, e2)))

test()
test2()

#endif
