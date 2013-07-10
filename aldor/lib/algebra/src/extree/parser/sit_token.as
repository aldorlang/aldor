------------------------------- sit_token.as ----------------------------------
--
-- Tokens for the lexical scanner
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#include "alg_tokens"

macro {
	Leaf	== ExpressionTreeLeaf;
	OP 	== ExpressionTreeOperator;
	CHAR	== Character;
	TEXT	== TextWriter;
}

#if GMP
macro FLOAT == Float;
#else
macro FLOAT == DoubleFloat;
#endif

#if ALDOC
\thistype{Token}
\History{Manuel Bronstein}{21/11/95}{created}
\Usage{import from \this}
\Descr{\this~is a type whose elements are parser tokens.}
\begin{exports}
\category{\altype{OutputType}}\\
\category{\altype{PrimitiveType}}\\
\alexp{float}:
& (\altype{List} \altype{Character}, \altype{List} \altype{Character}) $\to$ \% & Create a float token\\
\alexp{integer}:
& \altype{List} \altype{Character} $\to$ \% & Create an integer token\\
\alexp{leaf}: &	\% $\to$ \altype{ExpressionTreeLeaf} & Conversion to a leaf\\
\alexp{leaf?}: & \% $\to$ \altype{Boolean} & Test for a leaf\\
\alexp{name}:
& \altype{List} \altype{Character} $\to$ \% & Create a constant name token\\
\alexp{operator}:
& \% $\to$ \altype{ExpressionTreeOperator} & Conversion to an operator\\
\alexp{operator?}: & \% $\to$ \altype{Boolean} & Test for an operator\\
\alexp{prefix}:
& \altype{List} \altype{Character} $\to$ \% & Create a prefix function token\\
\alexp{special}:
& \% $\to$ \altype{MachineInteger} & Conversion to a special token\\
\alexp{special?}: & \% $\to$ \altype{Boolean} & Test for a special token\\
\alexp{string}: & \altype{List} \altype{Character} $\to$ \% & Create a string\\
\alexp{token}: & \altype{Character} $\to$ \altype{Partial} \% &
Create a single character token\\
\end{exports}
#endif

Token: Join(OutputType, PrimitiveType) with {
	float:	(List CHAR, List CHAR) -> %;
#if ALDOC
\alpage{float}
\Usage{\name($l_1,l_2$)}
\Signature{(\altype{List} \altype{Character}, \altype{List} \altype{Character})}{\%}
\Params{
$[d_0,\dots,d_n]$ & \altype{List} \altype{Character} & A list of digits\\
$[e_0,\dots,e_m]$ & \altype{List} \altype{Character} & A list of digits\\
}
\Retval{\name($[d_0,\dots,d_n], [e_0,\dots,e_m]$) returns the float
$d_n d_{n-1} \dots d_0 . e_m e_{m-1} \dots e_0$ as a token.}
\alseealso{\alexp{integer}}
#endif
	integer:	List CHAR -> %;
#if ALDOC
\alpage{integer}
\Usage{\name~l}
\Signature{\altype{List} \altype{Character}}{\%}
\Params{
$[d_0,\dots,d_n]$ & \altype{List} \altype{Character} & A list of digits\\ }
\Retval{\name($[d_0,\dots,d_n]$) returns the integer
$d_0 + 10 d_1 + \dots + 10^n d_n$ as a token.}
\alseealso{\alexp{float}}
#endif
	leaf:		% -> Leaf;
	leaf?:		% -> Boolean;
#if ALDOC
\alpage{leaf}
\altarget{\name?}
\Usage{ \name~t\\ \name?~t }
\Signatures{
\name: & \% $\to$ \altype{ExpressionTreeLeaf}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em t} & \% & A token\\ }
\Retval{
\name~a returns $a$ as a leaf is $a$ is a leaf.
\name?~a returns \true~if a is a leaf, \false~otherwise.
}
\alseealso{\alexp{operator}, \alexp{special}}
#endif
	name:		List CHAR -> %;
#if ALDOC
\alpage{name}
\Usage{\name~l}
\Signature{\altype{List} \altype{Character}}{\%}
\Params{
$[c_0,\dots,c_n]$ & \altype{List} \altype{Character} & A list of characters\\ }
\Retval{\name($[c_0,\dots,c_n]$) returns the name $c_n c_{n-1} \dots c_0$
as a token representing a constant symbol.}
\alseealso{\alexp{prefix}, \alexp{string}}
#endif
	operator:	% -> OP;
	operator?:	% -> Boolean;
#if ALDOC
\alpage{operator}
\altarget{\name?}
\Usage{ \name~t\\ \name?~t }
\Signatures{
\name: & \% $\to$ \altype{ExpressionTreeOperator}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em t} & \% & A token\\ }
\Retval{
\name~a returns $a$ as an operator is $a$ is an operator.
\name?~a returns \true~if a is an operator, \false~otherwise.
}
\alseealso{\alexp{leaf}, \alexp{special}}
#endif
	prefix:		List CHAR -> %;
#if ALDOC
\alpage{prefix}
\Usage{\name~l}
\Signature{\altype{List} \altype{Character}}{\%}
\Params{
$[c_0,\dots,c_n]$ & \altype{List} \altype{Character} & A list of characters\\ }
\Retval{\name($[c_0,\dots,c_n]$) returns the name $``c_n c_{n-1} \dots c_0''$
as a token representing a prefix function.}
\alseealso{\alexp{name}}
#endif
	special:	% -> TOKEN;
	special?:	% -> Boolean;
#if ALDOC
\alpage{special}
\altarget{\name?}
\Usage{ \name~t\\ \name?~t }
\Signatures{
\name: & \% $\to$ \altype{MachineInteger}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em t} & \% & A token\\ }
\Retval{
\name~a returns $a$ as a special token is $a$ is a special token.
\name?~a returns \true~if a is a special token, \false~otherwise.
}
\alseealso{\alexp{leaf}, \alexp{special}}
#endif
	string:		List CHAR -> %;
#if ALDOC
\alpage{string}
\Usage{\name~l}
\Signature{\altype{List} \altype{Character}}{\%}
\Params{
$[c_0,\dots,c_n]$ & \altype{List} \altype{Character} & A list of characters\\ }
\Retval{\name($[c_0,\dots,c_n]$) returns the string $``c_n c_{n-1} \dots c_0''$
as a token representing a constant.}
\alseealso{\alexp{name}, \alexp{prefix}}
#endif
	token:		OP -> %;
	token:		MachineInteger -> %;
	token:		CHAR -> Partial %;
#if ALDOC
\alpage{token}
\Usage{\name~a}
\Signatures{
\name: & \altype{Character} $\to$ \altype{Partial} \%\\
\name: & \altype{ExpressionTreeOperator} $\to$ \altype{Partial} \%\\
\name: & \altype{MachineInteger} $\to$ \altype{Partial} \%\\
}
\Params{
{\em a} & \altype{Character} & A single character token\\
        & \altype{ExpressionTreeOperator} & An operator\\
        & \altype{MachineInteger} & A code of a special token\\
}
\Retval{Returns the token corresponding to the single character $a$,
\failed if there is none.}
#endif
} == add {
	Rep == Union(uleaf: Leaf, uop: OP, utok: TOKEN);

	import from Rep;

	sample:%			== per [TOK__EOF];
	leaf?(t:%):Boolean		== rep(t) case uleaf;
	leaf(t:%):Leaf			== { assert(leaf? t); rep(t).uleaf; }
	operator?(t:%):Boolean		== rep(t) case uop;
	operator(t:%):OP		== { assert(operator? t); rep(t).uop; }
	special?(t:%):Boolean		== rep(t) case utok;
	special(t:%):TOKEN		== { assert(special? t); rep(t).utok; }
	token(n:MachineInteger):%	== per [n];
	token(op:OP):%			== per [op];
	local token(l:Leaf):%		== per [l];
	string(l:List CHAR):%		== token leaf revstring l;
	name(l:List CHAR):%		== token leaf symbol l;
	prefix(l:List CHAR):%		== token ExpressionTreePrefix symbol l;

	local opout(p:TEXT, t:%):TEXT == {
		import from Symbol;
		p << name$operator(t);
	}

	local opeq(t:%, s:%):Boolean == {
		import from MachineInteger;
		uniqueId$operator(t) = uniqueId$operator(s);
	}

	(p:TEXT) << (t:%):TEXT == {
		leaf? t => p << leaf t;
		operator? t => opout(p, t);  -- TEMPORARY (BUG 911)
		p << special t;
	}

	(x:%) = (y:%):Boolean == {
		leaf? x => leaf? y and leaf x = leaf y;
		special? x => special? y and special x = special y;
		operator? x and operator? y and opeq(x, y) -- TEMPORARY (911)
	}

	token(c:CHAR):Partial % == {
		import from String;
		c = char "^" => [token ExpressionTreeExpt];
		c = char "-" => [token ExpressionTreeMinus];
		c = char "+" => [token ExpressionTreePlus];
		c = char "/" => [token ExpressionTreeQuotient];
		c = char "*" => [token ExpressionTreeTimes];
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

	-- l = list of digits from low to high
	integer(l:List CHAR):% == {
		import from Boolean;
		assert(~empty? l);
		(n, e) := integerValue l;
		token leaf n;
	}

	-- l = list of digits from low to high
	-- returns (n, e) where n is the value of l
	-- and e = 10^m such that 10^{m-1} <= n < 10^m
	integerValue(l:List CHAR):(Integer, Integer) == {
		import from Boolean, CHAR, String, MachineInteger;
		assert(~empty? l);
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

	-- before, after = lists of digits from low to high
	float(before:List CHAR, after:List CHAR):% == {
		import from Boolean, FLOAT;
		assert(~empty? before);
		(x, e) := floatValue before;
		if ~empty?(after) then {
			(m, e) := floatValue after;
			x := x + (m / e);
		}
		token leaf x;
	}

	-- l = list of digits from low to high
	-- returns (n, e) where n is the value of l
	-- and e = 10^m such that 10^{m-1} <= n < 10^m
	local floatValue(l:List CHAR):(FLOAT, FLOAT) == {
		import from Boolean, CHAR, String, MachineInteger;
		assert(~empty? l);
		n:FLOAT := 0;
		ten := 10::FLOAT;
		pow10:FLOAT := 1;
		ord0 := ord char "0";
		while ~empty? l repeat {
			n := n + (ord(first l) - ord0)::FLOAT * pow10;
			pow10 := ten * pow10;
			l := rest l;
		}
		(n, pow10);
	}

	-- l = list of chars from right to left
	local symbol(l:List CHAR):Symbol == - revstring l;
	local revstring(l:List CHAR):String == {
		import from Boolean, MachineInteger, CHAR;
		assert(~empty? l);
		a:String := new(n := #l, null);
		for i in 1..n repeat {
			a(n-i) := first l;
			l := rest l;
		}
		a;
	}
}
