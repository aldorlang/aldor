--* From bronstei@inf.ethz.ch  Fri Jul 11 15:44:40 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA23122; Fri, 11 Jul 97 15:44:40 +0100
--* Received: from inf.ethz.ch (root@neptune.ethz.ch [129.132.10.10])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with SMTP
--* 	  id PAA18357 for <ax-bugs@nag.co.uk>; Fri, 11 Jul 1997 15:45:33 +0100 (BST)
--* Received: from ru8.inf.ethz.ch (bronstei@ru8.inf.ethz.ch [129.132.12.17]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id QAA24787 for <ax-bugs@nag.co.uk>; Fri, 11 Jul 1997 16:44:01 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (from bronstei@localhost) by ru8.inf.ethz.ch (8.8.4/8.7.1) id QAA13443 for ax-bugs@nag.co.uk; Fri, 11 Jul 1997 16:44:00 +0200 (MET DST)
--* Date: Fri, 11 Jul 1997 16:44:00 +0200 (MET DST)
--* Message-Id: <199707111444.QAA13443@ru8.inf.ethz.ch>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] the optimizer strikes back

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q2 badopt.as
-- Version: 1.1.9d
-- Original bug file name: badopt.as

------------------------------- badopt.as ----------------------------------
--
-- With 1.1.9d, this compiles ok at -q1, goes "forever" at -q2
--
-- With 1.1.9,  this compiles ok at all -q levels
--

#include "axllib"

macro {
	Z	== SingleInteger;
	OP	== ExpressionTreeOperator;
	TEXT	== TextWriter;
	TREE	== ExpressionTree;
	CHAR	== Character;
	PCHAR	== Partial Character;
	READ	== TextReader;
	Leaf	== ExpressionTreeLeaf;
}

ExpressionTreeOperator: Category == with {
	arity:		Z;
	asharp:		(TEXT, List TREE) -> TEXT;
	axiom:		(TEXT, List TREE) -> TEXT;
	C:		(TEXT, List TREE) -> TEXT;
	fortran:	(TEXT, List TREE) -> TEXT;
	lisp:		(TEXT, List TREE) -> TEXT;
	maple:		(TEXT, List TREE) -> TEXT;
	name:		String;
	tex:		(TEXT, List TREE) -> TEXT;
	texParen?:	Z -> Boolean;
	uniqueId:	Z;

	default {
		lisp(p:TEXT, l:List TREE):TEXT == {
			p := p << "(" << name;
			for arg in l repeat p := lisp(p << " ", arg);
			p << ")";
		}
	}
}

ExpressionTreeLeaf: BasicType with {
	asharp:		(TEXT, %) -> TEXT;
	axiom:		(TEXT, %) -> TEXT;
	boolean:	% -> Boolean;
	boolean?:	% -> Boolean;
	C:		(TEXT, %) -> TEXT;
	doubleFloat:	% -> DoubleFloat;
	doubleFloat?:	% -> Boolean;
	float:		% -> Float;
	float?:		% -> Boolean;
	fortran:	(TEXT, %) -> TEXT;
	integer:	% -> Integer;
	integer?:	% -> Boolean;
	leaf:		Boolean -> %;
	leaf:		SingleInteger -> %;
	leaf:		DoubleFloat -> %;
	leaf:		Integer -> %;
	leaf:		Float -> %;
	leafString:	String -> %;
	leafSymbol:	String -> %;
	lisp:		(TEXT, %) -> TEXT;
	maple:		(TEXT, %) -> TEXT;
	negate:		% -> %;
	negative?:	% -> Boolean;
	singleInteger:	% -> SingleInteger;
	singleInteger?:	% -> Boolean;
	string:		% -> String;
	string?:	% -> Boolean;
	symbol:		% -> String;
	symbol?:	% -> Boolean;
	tex:		(TEXT, %) -> TEXT;
	texParen?:	% -> Boolean;
} == add {
	macro {
		Str == Record(symb?: Boolean, str: String);
		Rep == Union(ubool: Boolean, usint: SingleInteger,
				udblf: DoubleFloat, uint: Integer,
				ufloat: Float, ustr: Str);
	}

	import from Rep;

	sample:%			== leaf(1@SingleInteger);
	leaf(n:SingleInteger):%		== per [n];
	leaf(x:DoubleFloat):%		== per [x];
	leaf(n:Integer):%		== per [n];
	leaf(x:Float):%			== per [x];
	leaf(b:Boolean):%		== per [b];
	leafString(s:String):%		== per [[false, s]];
	leafSymbol(s:String):%		== per [[true, s]];
	singleInteger?(l:%):Boolean	== rep(l) case usint;
	integer?(l:%):Boolean		== rep(l) case uint;
	doubleFloat?(l:%):Boolean	== rep(l) case udblf;
	float?(l:%):Boolean		== rep(l) case ufloat;
	boolean?(l:%):Boolean		== rep(l) case ubool;
	boolean(l:%):Boolean		== rep(l).ubool;
	integer(l:%):Integer		== rep(l).uint; 
	float(l:%):Float		== rep(l).ufloat;
	(p:TEXT) << (l:%):TEXT		== str(p, l, "_"", "_"");
	tex(p:TEXT, l:%):TEXT		== str(p, l, "``", "''");
	maple(p:TEXT, l:%):TEXT		== str(p, l, "`", "`");
	axiom(p:TEXT, l:%):TEXT		== p << l;
	asharp(p:TEXT, l:%):TEXT	== p << l;
	fortran(p:TEXT, l:%):TEXT	== C(p, l);
	texParen?(l:%):Boolean		== false;
	string(l:%):String		== rep(l).ustr.str;
	symbol(l:%):String		== rep(l).ustr.str;
	string?(l:%):Boolean	== rep(l) case ustr and ~rep(l).ustr.symb?;
	symbol?(l:%):Boolean	== rep(l) case ustr and rep(l).ustr.symb?;
	doubleFloat(l:%):DoubleFloat	== rep(l).udblf;
	singleInteger(l:%):SingleInteger== rep(l).usint;

	C(p:TEXT, l:%):TEXT == {
		boolean? l => {
			boolean l => p << "1";
			p << "0";
		}
		p << l;
	}

	lisp(p:TEXT, l:%):TEXT == {
		boolean? l => {
			boolean l => p << "t";
			p << "nil";
		}
		p << l;
	}

	(a:%) = (b:%):Boolean == {
		import from SingleInteger, String, Integer, Float, DoubleFloat;
		integer? a => integer? b and integer a = integer b;
		singleInteger? a =>
			singleInteger? b and singleInteger a = singleInteger b;
		float? a => float? b and float a = float b;
		doubleFloat? a =>
			doubleFloat? b and doubleFloat a = doubleFloat b;
		string? a => string? b and string a = string b;
		symbol? a => symbol? b and symbol a = symbol b;
		boolean? a and boolean? b and boolean a = boolean b;
	}

	local str(p:TEXT, l:%, opq:String, clq:String):TEXT == {
		singleInteger? l => p << singleInteger l;
		doubleFloat? l => p << doubleFloat l;
		integer? l => p << integer l;
		float? l => p << float l;
		boolean? l => p << boolean l;
		symbol? l => p << symbol l;
		p << opq << string l << clq;
	}

	negative?(l:%):Boolean == {
		singleInteger? l => singleInteger l < 0;
		doubleFloat? l => doubleFloat l < 0;
		integer? l => integer l < 0;
		float? l => float l < 0;
		false;
	}

	negate(l:%):% == {
		singleInteger? l => leaf(- singleInteger l);
		doubleFloat? l => leaf(- doubleFloat l);
		integer? l => leaf(- integer l);
		float? l => leaf(- float l);
		l;
	}
}

ExpressionTree: BasicType with {
	apply:		(OP, List %) -> %;
	arguments:	% -> List %;
	asharp:		(TEXT, %) -> TEXT;
	axiom:		(TEXT, %) -> TEXT;
	C:		(TEXT, %) -> TEXT;
	extree:		ExpressionTreeLeaf -> %;
	extreeSymbol:	String -> %;
	fortran:	(TEXT, %) -> TEXT;
	is?:		(%, OP) -> Boolean;
	leaf:		% -> ExpressionTreeLeaf;
	leaf?:		% -> Boolean;
	lisp:		(TEXT, %) -> TEXT;
	maple:		(TEXT, %) -> TEXT;
	negate:		% -> %;
	negative?:	% -> Boolean;
	operator:	% -> OP;
	tex:		(TEXT, %) -> TEXT;
	texParen?:	(Z, %) -> Boolean;
} == add {
	macro {
		Tree == Record(oper:OP, argum: List %);
		Rep == Union(uleaf: Leaf, utree: Tree);
	}

	import from Rep;

	sample:%			== per [sample$Leaf];
	extree(l:Leaf):%		== per [l];
	local tree(r:Tree):%		== per [r];
	extreeSymbol(s:String):%	== extree leafSymbol s;
	apply(op:OP, l:List %):%	== tree [op, l];
	leaf?(t:%):Boolean		== rep(t) case uleaf;
	local tree(t:%):Tree		== rep(t).utree;
	leaf(t:%):Leaf			== rep(t).uleaf;
	operator(t:%):OP		== operator tree t;
	arguments(t:%):List %		== arguments tree t;
	local operator(t:Tree):OP	== t.oper;
	local arguments(t:Tree):List %	== t.argum;
	texParen?(p:Z, t:Tree):Boolean	== texParen?(p)$operator(t);
	tex(p:TEXT, t:%):TEXT		== switchon(p, t, tex, tex);
	axiom(p:TEXT, t:%):TEXT		== switchon(p, t, axiom, axiom);
	maple(p:TEXT, t:%):TEXT		== switchon(p, t, maple, maple);
	C(p:TEXT, t:%):TEXT		== switchon(p, t, C, C);
	fortran(p:TEXT, t:%):TEXT	== switchon(p, t, fortran, fortran);
	lisp(p:TEXT, t:%):TEXT		== switchon(p, t, lisp, lisp);
	asharp(p:TEXT, t:%):TEXT	== switchon(p, t, asharp, asharp);
	is?(t:%, op:OP):Boolean		== (~leaf? t) and is?(tree t, op);
	local tex(p:TEXT, t:Tree):TEXT	== tex(p, arguments t)$operator(t);
	local axiom(p:TEXT,t:Tree):TEXT	== axiom(p, arguments t)$operator(t);
	local maple(p:TEXT,t:Tree):TEXT	== maple(p, arguments t)$operator(t);
	local asharp(p:TEXT,t:Tree):TEXT== asharp(p, arguments t)$operator(t);
	local fortran(p:TEXT,t:Tree):TEXT== fortran(p, arguments t)$operator(t);
	local lisp(p:TEXT,t:Tree):TEXT	== lisp(p, arguments t)$operator(t);
	local C(p:TEXT, t:Tree):TEXT	== C(p, arguments t)$operator(t);
	local negate(t:Tree):%		== first arguments t;

	local is?(t:Tree, op:OP):Boolean == {
		import from Z;
		uniqueId$operator(t) = uniqueId$op;
	}

	local negative?(t:Tree):Boolean == {
		import from Z;
		uniqueId$operator(t) = 500;
	}

	local opeq(x:%, y:%):Boolean == {
		import from Z;
		uniqueId$operator(x) = uniqueId$operator(y);
	}

	negative?(t:%):Boolean == {
		leaf? t => negative? leaf t;
		negative? tree t;
	}

	negate(t:%):% == {
		leaf? t => extree negate leaf t;
		negate tree t;
	}

	(p:TEXT) << (t:%):TEXT == {
		leaf? t => p << leaf t;
		stream(p, tree t);
	}

	local stream(p:TEXT, t:Tree):TEXT == {
		p := p << "(" << name$operator(t);
		for a in arguments t repeat p := p << " " << a;
		p << ")";
	}

	(x:%) = (y:%):Boolean == {
		leaf? x => leaf? y and leaf x = leaf y;
		(~leaf? y) and arguments x = arguments y and opeq(x, y)
	}

	local switchon(p:TEXT, t:%, f:(TEXT, Leaf) -> TEXT,
		g:(TEXT, Tree) -> TEXT):TEXT == {
			leaf? t => f(p, leaf t);
			g(p, tree t);
	}

	texParen?(p:Z, t:%):Boolean == {
		leaf? t => texParen? leaf t;
		texParen?(p, tree t);
	}
}

SampleOperator:ExpressionTreeOperator == add {
	name:String			== "+";
	arity:Z				== -1;
	uniqueId:Z			== 100;
	texParen?(p:Z):Boolean		== p > 100;
	axiom(p:TEXT, l:List TREE):TEXT	== trav(p, l, axiom);
	asharp(p:TEXT,l:List TREE):TEXT	== trav(p, l, asharp);
	maple(p:TEXT, l:List TREE):TEXT	== trav(p, l, maple);
	C(p:TEXT, l:List TREE):TEXT	== trav(p, l, C);
	fortran(p:TEXT,l:List TREE):TEXT== trav(p, l, fortran);
	trav(p:TEXT, l:List TREE, f:(TEXT, TREE) -> TEXT):TEXT == p;

	tex(p:TEXT, l:List TREE):TEXT == {
		empty? l => p;
		empty? rest l => tex(p, first l);
		lp := "\left(";
		rp := "\right)";
		paren? := texParen?(100, first l);
		if paren? then p := p << lp;
		p := tex(p, first l);
		for t in rest l repeat {
			if paren? then p := p << rp;
			p := p << "+";
			tt := t;
			paren? := texParen?(100, tt);
			if paren? then p := p << lp;
			p := tex(p, tt);
		}
		paren? => p << rp;
		p;
	}
}

SumitReader: with {
	end?: % -> Boolean;
	push!: (CHAR, %) -> %;
	read!: % -> CHAR;
	reader: READ -> %;
} == add {
	macro Rep == Record(pushback:List CHAR, rdr:READ);

	import from Rep;

	eof:CHAR		== { import from SingleInteger; char 0; }
	reader(t:READ):%	== per [empty(), t];
	cache(t:%):List(CHAR)	== rep(t).pushback;
	txtreader(t:%):READ	== rep(t).rdr;
	empty?(t:%):Boolean	== empty? cache t;
	end?(t:%):Boolean	== empty? t and check? t;

	check?(t:%):Boolean == {
		c := readchar!(txtreader t, eof);
		c = eof => true;
		push!(c, t);
		false;
	}

	push!(c:CHAR, t:%):% == {
		rep(t).pushback := cons(c, cache t);
		t;
	}

	pop!(t:%):CHAR == {
		c := first(l := cache t);
		rep(t).pushback := rest l;
		c;
	}

	read!(t:%):CHAR == {
		empty? t => readchar! txtreader t;
		pop! t;
	}
}

macro {
	TOKEN		== SingleInteger;
	TOK__UNKNOWN	== 100;
	TOK__EOF	== 200;
	TOK__EOEXPR	== 210;
	TOK__LPAREN	== 300;
	TOK__RPAREN	== 301;
	TOK__LBRACKET	== 302;
	TOK__RBRACKET	== 303;
	TOK__LCURLY	== 304;
	TOK__RCURLY	== 305;
	TOK__COMMA	== 310;
}

Token: BasicType with {
	float:		(List CHAR, List CHAR) -> %;
	integer:	List CHAR -> %;
	leaf:		% -> Leaf;
	leaf?:		% -> Boolean;
	name:		List CHAR -> %;
	operator:	% -> OP;
	operator?:	% -> Boolean;
	prefix:		List CHAR -> %;
	special:	% -> TOKEN;
	special?:	% -> Boolean;
	token:		OP -> %;
	token:		SingleInteger -> %;
	token:		CHAR -> Partial %;
} == add {
	macro Rep == Union(uleaf: Leaf, uop: OP, utok: TOKEN);

	import from Rep;

	sample:%			== per [TOK__EOF];
	leaf?(t:%):Boolean		== rep(t) case uleaf;
	leaf(t:%):Leaf			== rep(t).uleaf;
	operator?(t:%):Boolean		== rep(t) case uop;
	operator(t:%):OP		== rep(t).uop;
	special?(t:%):Boolean		== rep(t) case utok;
	special(t:%):TOKEN		== rep(t).utok;
	local opout(p:TEXT, t:%):TEXT	== p << name$operator(t);
	token(n:SingleInteger):%	== per [n];
	token(op:OP):%			== per [op];
	local token(l:Leaf):%		== per [l];
	name(l:List CHAR):%		== token leafSymbol string l;
	prefix(l:List CHAR):%		== token SampleOperator;

	local opeq(t:%, s:%):Boolean == {
		import from SingleInteger;
		uniqueId$operator(t) = uniqueId$operator(s);
	}

	(p:TEXT) << (t:%):TEXT == {
		leaf? t => p << leaf t;
		operator? t => opout(p, t);
		p << special t;
	}

	(x:%) = (y:%):Boolean == {
		leaf? x => leaf? y and leaf x = leaf y;
		special? x => special? y and special x = special y;
		operator? x and operator? y and opeq(x, y);
	}

	token(c:CHAR):Partial % == {
		c = char "^" => [token SampleOperator];
		c = char "-" => [token SampleOperator];
		c = char "+" => [token SampleOperator];
		c = char "/" => [token SampleOperator];
		c = char "*" => [token SampleOperator];
		c = char "(" => [token TOK__LPAREN];
		c = char ")" => [token TOK__RPAREN];
		c = char "[" => [token TOK__LBRACKET];
		c = char "]" => [token TOK__RBRACKET];
		c = char "{" => [token TOK__LCURLY];
		c = char "}" => [token TOK__RCURLY];
		c = char ";" => [token TOK__EOEXPR];
		c = char "," => [token TOK__COMMA];
		failed;
	}

	integer(l:List CHAR):% == {
		(n, e) := integerValue l;
		token leaf n;
	}

	float(before:List CHAR, after:List CHAR):% == {
		import from Float;
		(n, e) := integerValue before;
		x := n::Float;
		if ~empty?(after) then {
			(m, e) := integerValue after;
			x := x + (m::Float / e);
		}
		token leaf x;
	}

	integerValue(l:List CHAR):(Integer, Integer) == {
		n:Integer := 0;
		pow10:Integer := 1;
		ord0 := ord char "0";
		while ~empty? l repeat {
			n := n + (ord(first l) - ord0)::Integer * pow10;
			pow10 := 10 * pow10;
			l := rest l;
		}
		(n, pow10);
	}

	local string(l:List CHAR):String == {
		import from SingleInteger, Array CHAR;
		a:Array(CHAR) := new(n := #l, first l);
		for i in 1..n repeat {
			a(n+1-i) := first l;
			l := rest l;
		}
		string a;
	}
}

macro STEXT == SumitReader;

Scanner: with { scan!: STEXT -> Token } == add {
	local colon:CHAR	== char ":";
	local equal:CHAR	== char "=";
	local star:CHAR		== char "*";
	local slash:CHAR	== char "/";
	local bslash:CHAR	== char "\";
	local comma:CHAR	== char ",";
	local lpar:CHAR		== char "(";
	local rpar:CHAR		== char ")";
	local lcurly:CHAR	== char "{";
	local rcurly:CHAR	== char "}";
	local dot:CHAR		== char ".";
	local spaces:List CHAR	== [space, newline, tab];

	local push!(c:CHAR, port:STEXT, t:Token):Token == { push!(c, port); t; }
					
	local skipnewlines!(port:STEXT):CHAR == {
		c:CHAR := newline;
		while c = newline repeat c := read! port;
		c;
	}

	local skipspaces!(port:STEXT):CHAR == {
		c:CHAR := newline;
		while member?(c, spaces) repeat c := read! port;
		c;
	}

	scan!(port:STEXT):Token == {
		import from TOKEN, List CHAR, Partial Token;
		c := skipspaces! port;
		end? port => token TOK__EOF;
		c = lcurly => retract token lpar;
		c = rcurly => retract token rpar;
		c = star => scan4 port;
		c = slash => scan5 port;
		c = bslash => scan7 port;
		failed?(u := token c) => {
			letter? c => scan1(port, [c]);
			digit? c => scan2(port, [c]);
			c = colon => scan3 port;
			token TOK__UNKNOWN;
		}
		retract u;
	}

	local scan1(port:STEXT, l:List CHAR):Token == {
		c := skipnewlines! port;
		digit? c or letter? c => scan1(port, cons(c, l));
		push!(c, port, name l);
	}

	local scan2(port:STEXT, l:List CHAR):Token == {
		digit?(c := skipnewlines! port) => scan2(port, cons(c, l));
		c = dot => scan9(port, l, empty());
		c = space => integer l;
		push!(c, port, integer l);
	}

	local scan3(port:STEXT):Token == {
		import from TOKEN;
		(c := read! port) = equal => token SampleOperator;
		push!(c, port, token TOK__UNKNOWN);
	}

	local scan4(port:STEXT):Token == {
		import from TOKEN;
		(c := read! port) = star => token SampleOperator;
		push!(c, port, token SampleOperator);
	}

	local scan5(port:STEXT):Token == {
		import from TOKEN;
		(c := read! port) = star => scan6 port;
		push!(c, port, token SampleOperator);
	}

	local scan6(port:STEXT):Token == {
		import from TOKEN;
		c := skipspaces! port;
		while ~end?(port) repeat {
			if c = star then {
				(c := read! port) = slash => return scan! port;
			}
			else c := skipspaces! port;
		}
		token TOK__EOF;
	}

	local scan7(port:STEXT):Token == {
		import from TOKEN, List CHAR, Partial Token;
		(c := skipnewlines! port) = comma => token SampleOperator;
		letter? c => scan8(port, [c]);
		failed?(u := token bslash) => token TOK__UNKNOWN;
		push!(c, port, retract u);
	}

	local scan8(port:STEXT, l:List CHAR):Token == {
		import from Partial Token, String;
		c := skipnewlines! port;
		digit? c or letter? c => scan8(port, cons(c, l));
		(s := stringrev l) = "over" =>
			push!(c, port, token SampleOperator);
		(c = lpar and s = "left") or (c = rpar and s = "right") =>
								retract token c;
		push!(c, port, name l);
	}

	local scan9(port:STEXT, before:List CHAR, l:List CHAR):Token == {
		digit?(c := skipnewlines! port) => scan9(port,before,cons(c,l));
		c = space => float(before, l);
		push!(c, port, float(before, l));
	}

	local stringrev(l:List CHAR):String == {
		import from SingleInteger;
		s:String := new(n := #l);
		for c in l repeat {
			s.n := c;
			n := prev n;
		}
		s;
	}
}
