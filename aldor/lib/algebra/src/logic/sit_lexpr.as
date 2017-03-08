#include "algebra"
#include "aldorio"

OrClause: Join(ExpressionType, TotallyOrderedType, HashType) with {
    clause: IndexedAtom -> %;
    _or: Tuple % -> Partial %;
    atom: % -> Partial IndexedAtom;
    atoms: % -> List IndexedAtom;

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

    hash(cl: %): MachineInteger == {
        import from Fold2(MachineInteger, MachineInteger);
	import from IndexedAtom;
	(hashCombine, 31)/(hash atom for atom in rep cl)
    }

    atoms(cl: %): List IndexedAtom == rep cl;

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

    (a: %) < (b: %): Boolean == {
        import from IndexedAtom;
        false? a => false;
	false? b => true;
	a1 := first rep a;
	b1 := first rep b;
	a1 < b1 => true;
	b1 < a1 => false;
	per rest rep a < per rest rep b;
    }

}


AndClause: Join(ExpressionType, TotallyOrderedType, HashType) with {
    clause: IndexedAtom -> %;
    _and: Tuple % -> Partial %;
    _and: Generator % -> Partial %;
    atom: % -> Partial IndexedAtom;
    atoms: % -> List IndexedAtom;

    true: () -> %;
    true?: % -> Boolean;

    implies?: (%, %) -> Boolean;
}
== add {
    Rep ==> List IndexedAtom;
    import from Rep;

    clause(a: IndexedAtom): % == per [a];
    true(): % == per [];
    true?(cl: %): Boolean == empty? rep cl;

    atom(cl: %): Partial IndexedAtom == {
        true? cl => failed;
	not empty? rest rep cl => failed;
	[first rep cl]
    }

    hash(cl: %): MachineInteger == {
        import from Fold2(MachineInteger, MachineInteger);
	import from IndexedAtom;
	(hashCombine, 31)/(hash atom for atom in rep cl)
    }

    atoms(cl: %): List IndexedAtom == rep cl;

    local safelyCons(a: IndexedAtom, b: Partial List IndexedAtom): Partial List IndexedAtom ==
        if failed? b then failed else [cons(a, retract b)];

    local merge(a: Rep, b: Rep): Partial Rep == {
        import from IndexedAtom, Integer;
        empty? a => [b];
        empty? b => [a];
        aatom := first a;
        batom := first b;
        if aatom = batom then safelyCons(aatom, merge(rest a, rest b))
        else if isNegation?(aatom, batom) then failed;
        else if index aatom > index batom then safelyCons(batom, merge(a, rest b))
        else safelyCons(aatom, merge(rest a, b));
    }

    local and2(a: %, b: %): Partial % == {
        import from Partial Rep;
        pr := merge(rep a, rep b);
        failed? pr => failed;
        [per retract pr];
    }

    _and(t: Tuple %): Partial % == {
       import from MachineInteger;
       acc := element(t, 1);
       for idx in 2..(length(t)) repeat {
           term := element(t, idx);
	   pacc := and2(acc, term);
	   failed? pacc => return failed;
	   acc := retract pacc;
       }
       [acc]
    }

    _and(g: Generator %): Partial % == {
       import from MachineInteger;
       acc: % := true();
       for term in g repeat {
	   pacc := and2(acc, term);
	   failed? pacc => return failed;
	   acc := retract pacc;
       }
       [acc]
    }


    implies?(lhs: %, rhs: %): Boolean == {
        import from IndexedAtom;
        lhsList := rep lhs;
        rhsList := rep rhs;
	while not empty? rhsList and not empty? lhsList repeat {
	    if first lhsList = first rhsList then {
	        lhsList := rest lhsList;
	        rhsList := rest rhsList;
	    }
	    else if first lhsList < first rhsList then {
	        lhsList := rest lhsList;
	    }
	    else {
	        return false;
	    }
	}
	return empty? rhsList;
    }

    (a: %) = (b: %): Boolean == rep a = rep b;

    extree(a: %): ExpressionTree == {
        import from List ExpressionTree, IndexedAtom;
        import from ListMapper(IndexedAtom, ExpressionTree);
        import from ExpressionTreeLeaf;
        empty? rep a => extree leaf(true);
        ExpressionTreeList(map(extree)(rep a));
    }

    (a: %) < (b: %): Boolean == {
        import from IndexedAtom;
	true? b => false;
        true? a => true;
	a1 := first rep a;
	b1 := first rep b;
	a1 < b1 => true;
	b1 < a1 => false;
	per rest rep a < per rest rep b;
    }

}


OrLogicExpression: Join(BooleanAlgebra, TotallyOrderedType, HashType) with {
    expression: IndexedAtom -> %;
    not?: % -> Boolean;
    and?: % -> Boolean;
    or?: % -> Boolean;

    clause?: % -> Boolean;
    clause: % -> OrClause;
    atoms: % -> List IndexedAtom;

    terms: % -> List %;

    _and: List % -> %;
    _or: List % -> %;

    evaluate: (T: BooleanAlgebra, IndexedAtom -> T) -> (% -> T);
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
    clause? expr: Boolean == rep(expr) case CL;
    expression(a: IndexedAtom): % == per [clause a];

    terms expr: List % == {
        import from List IndexedAtom;
        clause? expr => [expression atom for atom in atoms rep(expr).CL];
	and? expr => rep(expr).AND;
	or? expr => rep(expr).OR;
	not? expr => [rep(expr).NOT];
	never;
    }

    hash expr: MachineInteger == {
        import from Fold2(MachineInteger, MachineInteger);
	clause? expr => hash clause expr;
	and? expr or or? expr => (hashCombine, 31)/(hash term for term in terms expr);
	not? expr => hashCombine(6247, hash rep(expr).NOT);
	never;
    }

    atoms expr: List IndexedAtom == {
        assert clause? expr;
        atoms rep(expr).CL;
    }

    clause expr: OrClause == {
        not clause? expr => { stdout << "Not a Clause: " << expr << newline; never}
        rep(expr).CL;
    }

    true: % == per [ [], AND];
    false: % == per [ false(), CL];

    true? expr: Boolean == and? expr and empty? rep(expr).AND;
    false? expr: Boolean == clause? expr and false? rep(expr).CL;

    _and(l: List %): % == {
        acc: % := true;
	for term in l repeat {
	    acc := _and(acc, term);
	}
	acc;
    }
    _or(l: List %): % == {
        acc: % := false;
	for term in l repeat {
	    acc := _or(acc, term);
	}
	acc;
    }

    _not(e: %): % == {
    	true? e => false;
	false? e => true;
        not? e => rep(e).NOT;
	per [e, NOT];
    }

    _and(expr1: %, expr2: %): % == {
        true? expr1 => expr2;
	false? expr1 => expr1;
        true? expr2 => expr1;
	false? expr2 => expr2;
        and? expr1 and and? expr2 => per [append!(copy rep(expr1).AND, rep(expr2).AND), AND];
	and? expr1 => per [append!(copy rep(expr1).AND, expr2), AND];
	and? expr2 => per [cons(expr1, rep(expr2).AND), AND];
	per [[expr1, expr2], AND];
    }
    
    _or(expr1: %, expr2: %): % == {
        import from Partial OrClause;
	true? expr1 => expr1;
	false? expr1 => expr2;
	true? expr2 => expr2;
	false? expr2 => expr1;
        clause? expr1 and clause? expr2 => {
	    tmp := _or(rep(expr1).CL, rep(expr2).CL);
	    failed? tmp => true;
	    per [retract tmp, CL]
	}
        or? expr1 and or? expr2 => per [append!(copy terms expr1, terms expr2), OR];
	per [[expr1, expr2], OR];
    }

    local validate(e: %): () == {
        true? e => return;
        false? e => return;
	clause? e => return;
	not? e => validate(rep(e).NOT);
	or? e => if empty? rest terms e then never;
	and? e => if empty? rest terms e then never;
	never;
    }

    extree(a: %): ExpressionTree == {
        import from List ExpressionTree;
        import from ExpressionTreeLeaf;
        import from ListMapper(%, ExpressionTree);
	validate(a);
        clause? a => extree(rep(a).CL);
        or? a => ExpressionTreeTimes(map(extree) rep(a).OR);
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
	clause? expr2 => false;
	or? expr1 => or? expr2 and rep(expr1).OR = rep(expr2).OR;
	not? expr1 => not? expr2 and rep(expr1).NOT = rep(expr2).NOT;
	false
    }

    local listCompare(l1: List %, l2: List %): Boolean == {
        empty? l1 and empty? l2 => false;
	empty? l1 => true;
	empty? l2 => false;
	first l1 < first l2 => true;
	first l2 < first l1 => false;
	listCompare(rest l1, rest l2);
    }

    (a: %) < (b: %): Boolean == {
        r := lessThan(a, b);
	r
    }

    local orderForType(a: %): Integer == {
        false? a => 0;
	true? a => 5;
	not? a => 1;
	clause? a => 2;
	or? a => 3;
	and? a => 4;
	never;
    }

    lessThan(a: %, b: %): Boolean == {
        import from Integer;
        orderForType a < orderForType b => true;
        orderForType b < orderForType a => false;
	orderForType a ~= orderForType b => never;
        clause? a and clause? b => clause a < clause b;
	not? a and not? b => _not a < _not b;
	and? a and and? b => listCompare(terms a, terms b);
	or? a and or? b => listCompare(terms a, terms b);
	never;
    }

    evaluate(T: BooleanAlgebra, mapfn: IndexedAtom -> T): % -> T == {
        import from T;
	import from Fold2(T, T);
	import from ListMapper(IndexedAtom, T);
	import from ListMapper(%, T);
        fn(expr: %): T == {
	    clause? expr => (_or, false)/map(mapfn) atoms clause expr;
	    not? expr => _not fn rep(expr).NOT;
	    and? expr => (_and, true)/map(fn) terms expr;
	    or? expr => (_or, false)/map(fn) terms expr;
	    never;
	}
        fn;
    }
}


LogicExpression: Join(BooleanAlgebra, TotallyOrderedType, HashType) with {
    expression: IndexedAtom -> %;
    not?: % -> Boolean;
    and?: % -> Boolean;
    or?: % -> Boolean;

    clause?: % -> Boolean;
    clause: % -> AndClause;
    atoms: % -> List IndexedAtom;

    terms: % -> List %;

    _and: List % -> %;
    _or: List % -> %;

    evaluate: (T: BooleanAlgebra, IndexedAtom -> T) -> (% -> T);
}
== add {
    Rep ==> Union(AND: List %, OR: List %, NOT: %, CL: AndClause);
    import from Rep;
    import from AndClause;
    import from List %;
    default expr: %;

    not? expr: Boolean == rep(expr) case NOT;
    and? expr: Boolean == rep(expr) case AND or rep(expr) case CL;
    or? expr: Boolean == rep(expr) case OR;
    clause? expr: Boolean == rep(expr) case CL;
    expression(a: IndexedAtom): % == per [clause a];

    terms expr: List % == {
        import from List IndexedAtom;
        clause? expr => [expression atom for atom in atoms rep(expr).CL];
	and? expr => rep(expr).AND;
	or? expr => rep(expr).OR;
	not? expr => [rep(expr).NOT];
	never;
    }

    hash expr: MachineInteger == {
        import from Fold2(MachineInteger, MachineInteger);
	clause? expr => hash clause expr;
	and? expr or or? expr => (hashCombine, 31)/(hash term for term in terms expr);
	not? expr => hashCombine(6247, hash rep(expr).NOT);
	never;
    }

    atoms expr: List IndexedAtom == {
        assert clause? expr;
        atoms rep(expr).CL;
    }

    clause expr: AndClause == {
        not clause? expr => { stdout << "Not a Clause: " << expr << newline; never}
        rep(expr).CL;
    }

    false: % == per [ [], OR];
    true: % == per [ true(), CL];

    true? expr: Boolean == clause? expr and true? rep(expr).CL;
    false? expr: Boolean == or? expr and empty? rep(expr).OR;

    _and(l: List %): % == {
        acc: % := true;
	for term in l repeat {
	    acc := _and(acc, term);
	}
	acc;
    }

    _or(l: List %): % == {
        acc: % := false;
	for term in l repeat {
	    acc := _or(acc, term);
	}
	acc;
    }

    _not(e: %): % == {
        true? e => false;
	false? e => true;
        not? e => rep(e).NOT;
	per [e, NOT];
    }

    _and(expr1: %, expr2: %): % == {
        import from Partial AndClause;
        true? expr1 => expr2;
	false? expr1 => expr1;
        true? expr2 => expr1;
	false? expr2 => expr2;
        clause? expr1 and clause? expr2 => {
	    tmp := _and(rep(expr1).CL, rep(expr2).CL);
	    failed? tmp => false;
	    per [retract tmp, CL]
	}
        and? expr1 and and? expr2 => per [append!(copy terms expr1, terms expr2), AND];
	and? expr1 => per [append!(copy terms expr1, [expr2]), AND];
	and? expr2 => per [cons(expr1, terms expr2), AND];
	per [[expr1, expr2], AND];
    }
    
    _or(expr1: %, expr2: %): % == {
	true? expr1 => expr1;
	false? expr1 => expr2;
	true? expr2 => expr2;
	false? expr2 => expr1;
        or? expr1 and or? expr2 => per [append!(copy terms expr1, terms expr2), OR];
	or? expr1 => per [append!(copy terms expr1, [expr2]), OR];
	or? expr2 => per [cons(expr1, terms expr2), OR];
	per [[expr1, expr2], OR];
    }

    local validate(e: %): () == {
        true? e => return;
        false? e => return;
	clause? e => return;
	not? e => validate(rep(e).NOT);
	or? e => if empty? rest terms e then never;
	and? e => if empty? rest terms e then never;
	never;
    }

    extree(a: %): ExpressionTree == {
        import from List ExpressionTree;
        import from ExpressionTreeLeaf;
        import from ListMapper(%, ExpressionTree);
	validate(a);
        clause? a => extree(rep(a).CL);
        and? a => {
            empty? rep(a).AND => extree leaf false;
            ExpressionTreeTimes(map(extree) rep(a).AND);
        }
        or? a => ExpressionTreePlus(map(extree) rep(a).OR);
        not? a => ExpressionTreeMinus([extree(rep(a).NOT)]);
        never;
    }

    (expr1: %) = (expr2: %): Boolean == {
	clause? expr1 => clause? expr2 and rep(expr1).CL = rep(expr2).CL;
	clause? expr2 => false;
        and? expr1 => and? expr2 and rep(expr1).AND = rep(expr2).AND;
	or? expr1 => or? expr2 and rep(expr1).OR = rep(expr2).OR;
	not? expr1 => not? expr2 and rep(expr1).NOT = rep(expr2).NOT;
	false
    }

    local listCompare(l1: List %, l2: List %): Boolean == {
        empty? l1 and empty? l2 => false;
	empty? l1 => true;
	empty? l2 => false;
	first l1 < first l2 => true;
	first l2 < first l1 => false;
	listCompare(rest l1, rest l2);
    }

    (a: %) < (b: %): Boolean == {
        r := lessThan(a, b);
	r
    }

    local orderForType(a: %): Integer == {
        false? a => 0;
	true? a => 5;
	not? a => 1;
	clause? a => 2;
	or? a => 3;
	and? a => 4;
	never;
    }

   local lessThan(a: %, b: %): Boolean == {
        import from Integer;
        orderForType a < orderForType b => true;
        orderForType b < orderForType a => false;
	orderForType a ~= orderForType b => never;
        clause? a and clause? b => clause a < clause b;
	not? a and not? b => _not a < _not b;
	and? a and and? b => listCompare(terms a, terms b);
	or? a and or? b => listCompare(terms a, terms b);
	never;
    }

    evaluate(T: BooleanAlgebra, mapfn: IndexedAtom -> T): % -> T == {
        import from T;
	import from Fold2(T, T);
	import from ListMapper(IndexedAtom, T);
	import from ListMapper(%, T);
        fn(expr: %): T == {
	    clause? expr => (_and, true)/map(mapfn) atoms clause expr;
	    not? expr => _not fn rep(expr).NOT;
	    and? expr => (_and, true)/map(fn) terms expr;
	    or? expr => (_or, false)/map(fn) terms expr;
	    never;
	}
        fn;
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

orClauses2(): List OrClause ==
    import from OrClause
    import from IndexedAtom
    import from Partial OrClause
    import from Integer
    [ false(), clause atom 1, clause atom 2,
                          clause negate atom 1, clause negate atom 2,
                          retract _or(clause atom 1, clause atom 2),
                          retract _or(clause atom 1, clause negate atom 2),
                          retract _or(clause negate atom 1, clause atom 2),
                          retract _or(clause negate atom 1, clause negate atom 2)]

testOrClauseOrder(): () ==
    import from ListMapper(Integer, IndexedAtom)
    import from Assert OrClause
    import from List OrClause
    import from OrClause
    for e1 in orClauses2() repeat
        for e2 in orClauses2() repeat
            assertFalse(e1 < e2 and e2 < e1)
            if e1 = e2 then
                assertFalse(e1 < e2)
                assertFalse(e2 < e1)
            else
                assertFalse(e1 < e2 and e2 < e1)
                assertTrue(e1 < e2 or e2 < e1)

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

testSSet(): () ==
   import from Integer;
   import from IndexedAtom;
   import from LogicExpression
   import from MachineInteger
   import from Assert MachineInteger
   l: List LogicExpression := [ expression atom 1, expression atom 2, false,
                                expression atom 2, expression atom 1]
   ll: SortedSet LogicExpression := [x for x in l]
   stdout << ll << newline
   assertEquals(3, #ll)

testEvaluate(): () ==
   import from Integer
   import from IndexedAtom
   import from LogicExpression
   import from Bit, Assert Bit;
   import from Integer
   eval := evaluate(Bit, (a: IndexedAtom): Bit +-> (even? index a)::Bit)
   assertEquals(true, eval expression atom 2);
   assertEquals(false, eval expression atom 1);
   assertEquals(false, eval _not expression atom 2);
   assertEquals(true, eval _not expression atom 1);
   assertEquals(false, eval _and(expression atom 1, expression atom 2));
   assertEquals(true, eval _or(expression atom 1, expression atom 2));
   assertEquals(false, eval _and(expression atom 1, expression negate atom 2));


testImplies(): () ==
    import from AndClause
    import from IndexedAtom
    import from Partial AndClause
    import from Assert AndClause
    import from Integer
    cl := retract _and(clause atom 1, clause atom 3)

    assertTrue(implies?(cl, cl))
    assertTrue(implies?(cl,  clause atom 1))
    assertTrue(implies?(cl,  clause atom 3))
    assertFalse(implies?(cl, clause atom 4))
    assertFalse(implies?(cl, clause atom 2))
    assertFalse(implies?(cl, clause negate atom 1))

test()
test2()
testOrClauseOrder()
testSSet()

testEvaluate()
testImplies()
#endif
