#include "axllib.as"

#library PolyCat "polycat.ao"
import   from PolyCat;

macro SI == SingleInteger;
macro S == SingleInteger;

OrderedDirectProduct(dim: SI, lessThan?: (Array S, Array S) -> Boolean):
	OrderedDirectProductCat

== add {
	Rep == Array S;
	import from Rep;

	map(f:S-> S, v: %):% == {
		vv:Rep := new(dim,0);
		for i: SI in 1..dim repeat
			vv.i := f v.i;
		per vv;
	}

	map(f:(S,S) -> S, v1:%, v2:%): % == {
		vv:Rep := new(dim,0);
		for i: SI in 1..dim repeat
			vv.i := f(v1.i, v2.i);
		per vv
	}
	vector(ts : Tuple S): % == {
                w := new(dim,0);
                for i in 1..min(length ts,dim) repeat w.i := element(ts, i);
                per w
	}
	unitVector(i:SI):% == {
		v:= new(dim,0);
		v.i := 1;
		per v
	}
	0: % == per new(dim,0);
	1: % == per new(dim,1);

	(v1: %) =  (v2: %): Boolean == rep(v1) = rep(v2);
	(v1: %) ~= (v2: %): Boolean == rep(v1) ~= rep(v2);
	(v1: %) <  (v2: %): Boolean == lessThan?(rep(v1), rep(v2));
	(v1: %) >  (v2: %): Boolean == v2 < v1;
	(v1: %) <= (v2: %): Boolean == not(v2 < v1);
	(v1: %) >= (v2: %): Boolean == not(v1 < v2);
	max(v1: %, v2: %): %    == if v1 > v2 then v1 else v2;
	min(v1: %, v2: %): %    == if v1 < v2 then v1 else v2;
	zero?(v: %): Boolean        == v = 0;

	(v1: %) + (v2: %): % == map(+, v1, v2);
	(v1: %) - (v2: %): % == map(-, v1, v2);
	+(v: %): %           == v;
	-(v: %): %           == map(-, v);
	sum(v: %): S == {
		s: S := 0;
		for vv in rep v repeat s := s + vv;
		s
	}
	sup(v1: %, v2: %): % ==
		map((%1: S,%2: S): S +-> if %1 < %2 then %2 else %1, v1, v2);
	apply(v:%, s:SI):S == rep(v).s;
	nonNegative?(v:%):Boolean == {
		for i:SI in 1..dim repeat
			if v(i)<0 then return false;
		true
	}
	(p: TextWriter) <<(v: %): TextWriter == {
	       dim=0 => p << "()";
	       p << "(" << v.1;
	       for i in 2..dim repeat p << "," << v.i;
	       p << ")"
	}
}

 
macro {
	qelt == apply;
	VS   == Array SS;
}

OrderingFunctions(dim: SI, SS: OrderedAbelianMonoid): with {
	pureLex    :  (VS,VS)  -> Boolean; ++ pure lexicographical ordering
	totalLex   :  (VS,VS)  -> Boolean; ++ total ordering refined with lex
	reverseLex :  (VS,VS)  -> Boolean; ++ reverse lexicographical ordering
} 
== add {
	import from SS;
	n: SI := dim;
 
	pureLex(v1: VS, v2: VS): Boolean == {
		for i in 1..n repeat {
			if qelt(v1,i) < qelt(v2,i) then return true;
			if qelt(v2,i) < qelt(v1,i) then return false;
		}
		false
	}

	totalLex(v1: VS, v2: VS): Boolean == {
		n1: SS := 0;
		n2: SS := 0;
		for i in 1..n repeat {
			n1 := n1 + qelt(v1, i);
			n2 := n2 + qelt(v2, i);
		}
		n1 < n2 => true;
		n2 < n1 => false;
		for i in 1..n repeat {
			if qelt(v1, i) < qelt(v2, i) then return true;
			if qelt(v2, i) < qelt(v1, i) then return false;
		}
		false;
	}

	reverseLex(v1: VS, v2: VS): Boolean == {
		n1: SS := 0;
		n2: SS := 0;
		for i in 1..n repeat {
			n1 := n1 + qelt(v1, i);
			n2 := n2 + qelt(v2, i);
		}
		n1 < n2 => true;
		n2 < n1 => false;
		for i in n..1 by -1 repeat {
			if qelt(v2, i) < qelt(v1, i) then return true;
			if qelt(v1, i) < qelt(v2, i) then return false;
		}
		false
	}
}

HomogeneousDirectProduct(dim: SI): OrderedDirectProductCat ==
	OrderedDirectProduct(dim,reverseLex$OrderingFunctions(dim,SI)) add;

LexicographicDirectProduct(dim: SI): OrderedDirectProductCat ==
	OrderedDirectProduct(dim,pureLex$OrderingFunctions(dim,SI)) add;
