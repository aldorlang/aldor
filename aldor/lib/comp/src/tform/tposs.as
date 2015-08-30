#include "comp"
#include "aldorio"
#pile

TPoss: OutputType with
    empty: () -> %
    empty?: %  -> Boolean
    add!: (%, TForm) -> ()
    bracket: Generator TForm -> %
    bracket: Tuple TForm -> %
    generator: % -> Generator TForm
    crossProduct: List % -> %
    union: (%, %) -> %
    size: % -> MachineInteger
    unique: % -> Partial TForm
    unique?: % -> Boolean
== add
    Rep == Record(l: List TForm)
    import from Rep, TForm
    import from MachineInteger
    import from String
    
    empty(): % == per [[]]
    empty?(tp: %): Boolean == empty? rep(tp).l
    size(tp: %): MachineInteger == # rep(tp).l

    unique?(tp: %): Boolean ==
        import from Partial TForm
        not failed? unique tp

    unique(tp: %): Partial TForm ==
        empty? rep(tp).l => failed
	not empty? rest rep(tp).l => failed
	[first rep(tp).l]

    add!(tp: %, tf: TForm): () ==
        sz := size tp
        rep(tp).l := cons(tf, rep(tp).l)
	assert size tp = sz + 1

    local generator(t: Tuple TForm): Generator TForm == generate
        for i in 1..length(t) repeat yield element(t, i)

    bracket(t: Tuple TForm): % ==
        tp: % := [tf for tf in t]
	import from MachineInteger, String
	size tp ~= length t => error "???"
	tp

    bracket(g: Generator TForm): % ==
        tp := empty()
	for tf in g repeat add!(tp, tf)
	tp

    generator(tp: %): Generator TForm == generator rep(tp).l

    union(tp1: %, tp2: %): % == [tf for tf in concat(tp1, tp2)]

    crossProduct(l: List %): % ==
        import from TFormTagComma, List List TForm
        cp: % := [comma(tf for tf in tfList) for tfList in crossProduct0 l]
	stdout << "(CrossProd: " << l << newline
	stdout << "    ===> " << cp << newline
	return cp

    crossProduct0(tpossL: List %): List List TForm ==
        import from List TForm
	firstElt := first tpossL
	empty? rest tpossL =>
	    empty? firstElt => []
	    [[tf] for tf in rep(firstElt).l]
	restL := crossProduct0 rest tpossL
	append ([cons(poss, crossRest@List(TForm)) for poss in firstElt] for crossRest in restL)

    concat(g1: %, g2: %): Generator TForm == generate
        for x in g1 repeat yield x
        for x in g2 repeat yield x

    append(g: Generator List List TForm): List List TForm ==
        x: List List TForm := []
	for ll in g repeat x := append!(copy ll, x)
	x

    (o: TextWriter) << (tp: %): TextWriter ==
        o << "(";
	sep := ""
	for tf in tp repeat
	    o << sep << tf
	    sep := ", "
	o << ")"


#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

string(s: Literal): AbSyn ==
    import from Id
    id string s

string(s: Literal): Id == id string s

s(x: String): String == x

testTPoss(): () ==
    import from Assert MachineInteger
    import from MachineInteger
    import from TFormTagId, TFormTagComma
    import from List TPoss
    import from BooleanFold
    import from Partial TForm
    assertTrue(empty? empty()$TPoss)
    tp1: TPoss := [id "a"]
    tp2: TPoss := [id "b"]
    assertEquals(1, size tp1)
    assertEquals(1, size tp2)
    assertEquals(2, size union(tp1, tp2))

    assertEquals(1, size crossProduct [tp1, tp2])
    assertTrue(_and/(comma? tp for tp in crossProduct [tp1, tp2]))

    assertEquals(0, size crossProduct [empty()])
    assertEquals(1, size crossProduct [tp1])
    assertEquals(2, size crossProduct [union(tp1, tp2)])
    assertEquals(2, size crossProduct [tp1, union(tp1, tp2)])
    assertEquals(2, size crossProduct [union(tp1, tp2), tp1])
    assertEquals(4, size crossProduct [union(tp1, tp2), union(tp1, tp2)])

    assertTrue unique? tp1
    assertFalse unique? empty()
    assertFalse unique? union(tp1, tp2)

testTPoss()
#endif
