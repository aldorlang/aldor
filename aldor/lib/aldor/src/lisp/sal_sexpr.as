#include "aldor"
#include "aldorio"
#pile

local Cons: PrimitiveType with
    cons: (SExpression, SExpression) -> %
    first: % -> SExpression
    rest: % -> SExpression
    setFirst!: (%, SExpression) -> ()
    setRest!: (%, SExpression) -> ()
== add 
    Rep == Record(car: SExpression, cdr: SExpression)
    import from Rep
    default cons1, cons2: %
    
    first(cons: %): SExpression == rep(cons).car
    rest(cons: %): SExpression == rep(cons).cdr
    cons(car: SExpression, cdr: SExpression): % == per [car, cdr]

    setFirst!(cons: %, sx: SExpression): () == rep(cons).car := sx
    setRest!(cons: %, sx: SExpression): () == rep(cons).cdr := sx
    
    cons1 = cons2: Boolean ==
        first cons1 = first cons2 and rest cons1 = rest cons2

CharSets: with
    symStart?: Character -> Boolean
    symPart?: Character -> Boolean
    numberStart?: Character -> Boolean
    numberPart?: Character -> Boolean
    whitespace?: Character -> Boolean
== add
    default c: Character
    local symStarts: Set Character := 
        [char "+", char "-",
	 char "*", char "/",
	 char "<", char ">",
	 char "%", char "$",
	 char "~", char "?",
	 char "=",
	 char "^", char "__", char ":"]
    
    symStart?(c): Boolean == letter? c or member?(c, symStarts)
    whitespace?(c): Boolean == c = space or c = newline or c = tab
    symPart?(c): Boolean ==
        symStart? c or digit? c

    numberStart? c: Boolean == digit? c
    numberPart? c: Boolean == digit? c

SExpression: Join(InputType, OutputType, PrimitiveType) with
    sexpr: Symbol -> %
    sexpr: Integer -> %
    sexpr: String -> %
    sexpr: Cons -> %
    nil: %
    cons?: % -> Boolean
    int?: % -> Boolean
    str?: % -> Boolean
    sym?: % -> Boolean
    nil?: % -> Boolean

    cons: (%, %) -> %
    first: % -> %
    rest: % -> %
    bracket: Generator % -> %
    bracket: Tuple % -> %
    append: (%, %) -> %

    sym: % -> Symbol
    int: % -> Integer
    str: % -> String

    first: % -> %
    rest: % -> %

    nth: (%, Integer) -> %
    generator: % -> Generator %
== add 
    Rep == Union(SYM: Symbol, INT: Integer, STR: String, CONS: Cons)
    import from Rep
    default sx, sx1, sx2: %
    nil: % == (nil$Pointer) pretend %
    nil(): % == (nil$Pointer) pretend %

    sym(sx: %): Symbol == rep(sx).SYM
    int(sx: %): Integer == rep(sx).INT
    str(sx: %): String == rep(sx).STR

    sexpr(sym: Symbol): % == per [sym]
    sexpr(n: Integer): % == per [n]
    sexpr(str: String): % == per [str]
    sexpr(cons: Cons): % == per [cons]

    cons? sx: Boolean == not nil? sx and rep(sx) case CONS
    sym? sx: Boolean == not nil? sx and rep(sx) case SYM
    int? sx: Boolean == not nil? sx and rep(sx) case INT
    str? sx: Boolean == not nil? sx and rep(sx) case STR
    nil? sx: Boolean == {
        import from Pointer;
        (rep(sx) pretend Pointer) = nil
    }
    first sx: % == first rep(sx).CONS
    rest sx: % == rest rep(sx).CONS

    setRest!(sx, r: SExpression): () == setRest!(rep(sx).CONS, r)

    cons(sx1, sx2): % ==
        per [cons(sx1, sx2)]

    sx1 = sx2: Boolean ==
        cons? sx1 and cons? sx2 =>
	    rep(sx1).CONS = rep(sx2).CONS
	sym? sx1 and sym? sx2 =>
	    rep(sx1).SYM = rep(sx2).SYM
	int? sx1 and int? sx2 =>
	    rep(sx1).INT = rep(sx2).INT
	str? sx1 and str? sx2 =>
	    rep(sx1).STR = rep(sx2).STR
	nil? sx1 and nil? sx2 => true
	false

    (o: TextWriter) << (sx: %): TextWriter ==
	nil? sx => o << "()"
        cons? sx => writeList(o, sx)
	int? sx => o << rep(sx).INT
	str? sx => writeString(o, rep(sx).STR)
	sym? sx => writeSymbol(o, rep(sx).SYM)
	never

    bracket(t: Tuple %): % ==
        import from MachineInteger
	length(t) = 0 => nil
	length(t) = 1 => cons(element(t, 1), nil)
	l: % := nil
	for n in length(t)..1 by -1 repeat l := cons(element(t, n), l)
	l

    bracket(g: Generator %): % ==
        l := nil()
	last := nil()
	for sx in g repeat
	    if last = nil() then
	        l := cons(sx, nil())
		last := l
	    else
	        next: % := cons(sx, nil())
	        setRest!(last, next)
		last := next
	return l

    -- this is needed (by append) to avoid a of_emerge bug which
    -- merges a potentially null reference.
    local copyList(sx): (SExpression, SExpression) ==
        nil? sx => never
	last: % := cons(first sx, nil())
	result := last
	sx := rest sx
	while cons? sx repeat
	    next: % := cons(first sx, nil())
	    setRest!(last, next)
	    last := next
	    sx := rest sx
	if not nil? sx then never
	(result, last)

    append(sx1, sx2): % ==
        nil? sx1 => sx2
	(result, lastPair) := copyList(sx1)
	setRest!(lastPair, sx2)
	result

    nth(sx, n: Integer): % == if n = 0 then first sx else nth(rest sx, n-1)

    generator(sx): Generator % == generate {
        while cons? sx repeat {
	    yield first sx;
	    sx := rest sx
	}
    }

    local writeList(o: TextWriter, sx): TextWriter ==
        o << "("
	o << first sx
	sx := rest sx
	while cons? sx repeat
	    o << " " << first(sx)
	    sx := rest(sx)
	if not nil? sx then
	    o << " . " << sx
	o << ")"

    local writeString(o: TextWriter, s: String): TextWriter ==
        o << "_"" << s << "_""

    local writeSymbol(o: TextWriter, s: Symbol): TextWriter ==
        o << name s

    <<(rdr: TextReader): % ==
        import from SExpressionReader, Partial %
	retract read rdr

LStream(T: Type): Category == with
    peek: % -> T
    next!: % -> ()
    hasNext?: % -> Boolean

TextLStream: LStream Character with
    tstream: TextReader -> %;
== add
    Rep == Record(rdr: TextReader, curr: Character, hasCurr: Boolean, atEof: Boolean);
    import from Rep
    import from Character
    default str: %;

    local hasCurr? str: Boolean == rep(str).hasCurr

    tstream(rdr: TextReader): % == per [rdr, eof, false, false]

    local readOne(str): () ==
        c := read!(rep(str).rdr)
	if eof = c then
	    rep(str).atEof := true
	else
	    rep(str).curr := c
        rep(str).hasCurr := true

    peek(str): Character ==
        rep(str).atEof => never
        if not hasCurr? str then readOne str
	rep(str).atEof => never
	rep(str).curr

    hasNext?(str): Boolean ==
        if not hasCurr? str then readOne str
        not rep(str).atEof
    
    next!(str): () ==
        rep(str).hasCurr := false

FnLStream(T: Type): LStream T with
    tstream: (readOne: () -> Partial T) -> %
== add
    Rep == Record(readOne!: () -> Partial T,
    	          curr: Partial T, atEos: Boolean);
    import from Rep
    default str: %;

    tstream(readOne!: () -> Partial T): % == per [readOne!, failed, false]

    local readOne!(str): () ==
        item := rep(str).readOne!()
	if failed? item then rep(str).atEos := true
	rep(str).curr := item
	
    peek(str): T ==
        rep(str).atEos => never
        if failed? rep(str).curr then readOne!(str)
	retract rep(str).curr

    hasNext?(str): Boolean ==
	rep(str).atEos => false
        if failed? rep(str).curr then readOne!(str)
        not rep(str).atEos
    
    next!(str): () ==
        rep(str).curr := failed


SExpressionReader: with
   read: (TextReader) -> Partial SExpression;
   readCased: (TextReader)  -> Partial SExpression;
== add
    Token == Record(type: 'sym,escsym,number,str,ws,oparen,cparen,dot,quote,error,getref,setref', txt: String);
    import from Token
    import from CharSets

    read(rdr: TextReader): Partial SExpression == read(rdr, false)
    readCased(rdr: TextReader): Partial SExpression == read(rdr, true)

    local read(rdr: TextReader, cased: Boolean): Partial SExpression ==
        import from TextLStream
        import from FnLStream Token
        s := tstream rdr
        tokstrm := tstream((): Partial Token +->  readOneToken! s)
        sxMaybe: Partial SExpression := read(tokstrm, cased)
	sxMaybe
    
    local readOneToken(rdr: TextReader, cased: Boolean): Partial Token ==
        import from TextLStream
        s := tstream rdr
        if hasNext? s then readOneToken! s else failed

    local readOneToken!(s: TextLStream): Partial Token ==
        import from Character
	not hasNext? s => failed
        c := peek s;
	whitespace? c => [readWhitespace(s)]
	c = char "(" =>
	    next! s
	    [[oparen, c::String]]
	c = char ")" =>
	    next! s
	    [[cparen, c::String]]
	c = char "|" => [readEscaped s]
	c = char "." =>
	    next! s
	    [[dot, c::String]]
	c = char "'" =>
	    next! s
	    [[quote, c::String]]
	c = char "\" =>
            [readBackslashEscaped s]
	c = char "#" => [readReference s]
	c = char "_"" => [readString s]
	symStart? c => [readSymbol s]
	numberStart? c => [readNumber s]
	stdout << "Unknown token prefix " << c << newline
	failed

    local readString(s: TextLStream): Token ==
        buffer: StringBuffer := new()
        writer: TextWriter := coerce buffer
	next! s
        while hasNext? s and peek s ~= char "_"" repeat
	    if peek s = char "\" then
	        next! s
            writer << peek(s)
	    next! s
	    not hasNext? s => [error, "eof inside string"]
	not hasNext? s => [error, "eof inside string"]
	next! s
	[str, string buffer]

    local readReference(s: TextLStream): Token ==
        next! s
	text := ""
	while hasNext? s and digit? peek s repeat
	    text := text + peek(s)::String
	    next! s
	peek s = char "=" =>
	   next! s
	   return [setref, text]
	if peek s = char "#" then next! s
	[getref, text]

    local readWhitespace(s: TextLStream): Token ==
        buffer: StringBuffer := new()
        writer: TextWriter := coerce buffer
        import from Character
        writer << peek s
	next! s
	while whitespace? peek s repeat
	    writer << peek s
	    next! s
	[ws, string buffer]

    local readEscaped(s: TextLStream): Token ==
        import from Character
        buffer: StringBuffer := new()
        writer: TextWriter := coerce buffer
	next! s
	while peek s ~= char "|" repeat
	    if peek(s) = char "\" then
	        next! s
	    writer << peek s
	    next! s
	next! s
	[escsym, string buffer]

    local readBackslashEscaped(s: TextLStream): Token ==
        next! s
        text := peek(s)::String
        next! s
	[escsym, text]

    local readNumber(s: TextLStream): Token ==
        buffer: StringBuffer := new()
        writer: TextWriter := coerce buffer
	while hasNext? s and numberPart? peek s repeat
	    writer << peek(s)
	    next! s
	[number, string buffer]

    local readSymbol(s: TextLStream): Token ==
        buffer: StringBuffer := new()
        writer: TextWriter := coerce buffer
	while hasNext? s and symPart? peek s repeat
	    writer << peek(s)
	    next! s
	[sym, string buffer]

    local read(s: FnLStream Token, cased: Boolean): Partial SExpression ==
        import from SExpression, Symbol
	tbl: HashTable(String, SExpression) := table()
	setref!(id: String, psx: Partial SExpression): Partial SExpression ==
	    if not failed? psx then tbl.id := retract psx
	    psx
	getref(id: String): Partial SExpression == find(id, tbl)
        skipWhitespace!(): () ==
            while hasNext? s and peek(s).type = ws repeat
	        next! s

        readList(): Partial SExpression ==
	    not hasNext? s =>
		stdout << "no next" << newline
		failed
	    peek(s).type = cparen =>
	        next! s
	        [nil]
	    tmp := read()
	    failed? tmp =>
		stdout << "failed rec call" << newline
		failed
	    head: Cons := cons(retract tmp, nil)
	    last := head
	    done := false
	    while not done repeat
	        skipWhitespace!()
		not hasNext? s =>
		    stdout << "no next" << newline
		    return failed
	        if peek(s).type = dot then
		    next! s
		    final := read()
		    failed? final =>
		        return failed
		    setRest!(last, retract final)
		    skipWhitespace!()
		    if peek(s).type ~= cparen then
		        return failed
		    next! s
		    done := true
		else if peek(s).type = cparen then
		    done := true
		    next! s
		else
		    next := read()
		    failed? next => return failed
		    nextCell: Cons := cons(retract next, nil)
		    setRest!(last, sexpr nextCell)
		    last := nextCell
            return [sexpr head]

	readQuoted(): Partial SExpression ==
	    sx := read()
	    failed? sx => failed
	    [[sexpr(-"QUOTE"), retract sx]]
	    
        read(): Partial SExpression ==
	    import from Integer
            skipWhitespace!()
	    not hasNext? s =>
		stdout << "no next" << newline
	        failed
            tok := peek s;
	    next! s
	    if tok.type = oparen then readList()
	    else if tok.type = cparen then
	        stdout << "cparen" << newline
		failed
	    else if tok.type = str then [sexpr tok.txt]
	    else if tok.type = quote then readQuoted()
	    else if tok.type = sym then
	        [if cased then sexpr tok.txt else sexpr (-[upper x for x in tok.txt])]
	    else if tok.type = escsym then
	        [sexpr (-tok.txt)]
	    else if tok.type = number then [sexpr integer literal tok.txt]
	    else if tok.type = setref then
	        setref!(tok.txt, read())
	    else if tok.type = getref then
	        getref(tok.txt)
	    else if tok.type = error then
                stdout << "Error " << tok.txt << newline
		failed
	    else
	        stdout << "gawd knows" << tok.type << newline
	        failed
        read()

#if ALDORTEST
#include "aldor"
#include "aldorio"
#pile

readOne(s: String): Partial SExpression ==
    import from SExpressionReader
    sb: StringBuffer := new()
    sb::TextWriter << s
    read(sb::TextReader)

test(): () ==
    import from Partial SExpression
    import from SExpression
    import from Assert SExpression
    import from Integer
    import from Symbol

    sxMaybe := readOne("foo")
    assertFalse failed? sxMaybe
    foo := sexpr (-"FOO")
    assertEquals(foo, retract sxMaybe)

    sxMaybe := readOne("23")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr 23, retract sxMaybe)

    sxMaybe := readOne( "_"hello_"")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr "hello", retract sxMaybe)

    sxMaybe := readOne("(foo)")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(cons(foo, nil), retract sxMaybe)

    sxMaybe := readOne("(foo 2)")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(cons(foo, cons(sexpr 2, nil)), retract sxMaybe)

    sxMaybe := readOne("(foo . 2)")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(cons(foo, sexpr 2), retract sxMaybe)

    sxMaybe := readOne("|+->|")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr (-"+->"), retract sxMaybe)

    sxMaybe := readOne("(foo () 2)")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals([sexpr(-"FOO"), [], sexpr 2], retract sxMaybe)

    sxMaybe := readOne("symbol?")
    stdout << "SX: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr(-"SYMBOL?"), retract sxMaybe)

    sxMaybe := readOne("_"hello\_"_"")
    stdout << "strsx: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr("hello_""), retract sxMaybe)

    sxMaybe := readOne("_"\\_"")
    stdout << "strsx: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr("\"), retract sxMaybe)

    sxMaybe := readOne("|\||")
    stdout << "strsx: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr(-"|"), retract sxMaybe)

    sxMaybe := readOne("|__\|__|")
    stdout << "strsx: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals(sexpr(-"__|__"), retract sxMaybe)

    sxMaybe := readOne("'x")
    stdout << "strsx: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals([sexpr(-"QUOTE"), sexpr(-"X")], retract sxMaybe)

    sxMaybe := readOne("(((foo) . 1) ((|bar|) . 2))")
    stdout << "strsx: " << sxMaybe << newline
    assertFalse failed? sxMaybe
    assertEquals([cons([sexpr(-"FOO")], sexpr 1), cons([sexpr(-"bar")], sexpr 2)], retract sxMaybe)

test()

test2(): () ==
    import from File
    import from SExpression
    import from SExpressionReader
    import from Partial SExpression
    import from Assert SExpression

    rdr := open("sal__sexpr.asy")::TextReader
    
    sx := read(rdr)
    assertFalse(failed? sx)

testBracket(): () ==
    import from Assert SExpression
    import from Integer
    sx: SExpression := [sexpr x for x in 1..3]
    assertEquals(sexpr 1, first sx)
    assertEquals(sexpr 2, first rest sx)
    assertEquals(sexpr 3, first rest rest sx)
    assertEquals(nil, rest rest rest sx)

testAppend(): () ==
    import from Assert SExpression
    import from Integer
    sx1: SExpression := cons(sexpr 1, nil)
    sx2: SExpression := cons(sexpr 2, nil)
    assertEquals(sexpr 1, first append(nil, sx1))
    assertEquals(nil, rest append(nil, sx1))
    assertEquals(sexpr 1, first append(sx1, nil))
    assertEquals(sexpr 1, first append(sx1, sx2))
    assertEquals(sexpr 2, first rest append(sx1, sx2))

testNth(): () ==
    import from Assert SExpression
    import from SExpression
    import from Integer
    l: SExpression := [sexpr 1, sexpr 2, sexpr 3]
    assertEquals(sexpr 1, nth(l, 0))
    assertEquals(sexpr 2, nth(l, 1))
    assertEquals(sexpr 3, nth(l, 2))

testGenerator(): () ==
    import from Assert Integer
    import from SExpression
    import from Integer
    import from Fold Integer
    sx: SExpression := [sexpr n for n in 1..3]
    sum := (+)/(int elt for elt in sx)
    assertEquals(6, sum)

testReadRef(): () ==
    import from Assert SExpression
    import from Partial SExpression, SExpression, Symbol
    sxMaybe := readOne("(#1=(a) #1)")
    assertFalse failed? sxMaybe
    a: SExpression := [sexpr.(-"A")]
    assertEquals([a, a], retract sxMaybe)

testReadRef2(): () ==
    import from Assert SExpression
    import from Partial SExpression, SExpression, Symbol
    sxMaybe := readOne("(#1=(a) #1#)")
    assertFalse failed? sxMaybe
    a: SExpression := [sexpr.(-"A")]
    assertEquals([a, a], retract sxMaybe)

test2()
testBracket()
testAppend()
testNth()
testGenerator()
testReadRef()
testReadRef2()

import from Integer
nada: SExpression := cons(sexpr 1, sexpr 2)
testAppend2(): SExpression ==
    l := nada
    l2 := if cons? l then a := cons(sexpr 22, nil) else nil
    append(l2, l)

nada: SExpression := nil

testAppend2()

testFileStuff(): () ==
    import from SExpression, Symbol, MachineInteger
    import from Assert SExpression
    file: File := open("foo.lsp", fileWrite)
    w: TextWriter := file::TextWriter
    w <<  [sexpr(-"hello"), sexpr(-"world")]@SExpression << newline
    w <<  [sexpr(-"goodbye"), sexpr(-"world")]@SExpression << newline
    close! file

    infile := open("foo.lsp", fileRead)
    r := infile::TextReader
    sx: SExpression := << r
    assertEquals(sx, [sexpr(-"HELLO"), sexpr(-"WORLD")]@SExpression)

    infile := open("foo.lsp", fileRead)
    r := infile::TextReader
    setPosition!(infile, 14)
    sx: SExpression := << r
    assertEquals(sx, [sexpr(-"GOODBYE"), sexpr(-"WORLD")]@SExpression)

testFileStuff()

#endif
