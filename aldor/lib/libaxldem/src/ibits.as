#include "axllib.as"

macro {
	SI == SingleInteger;
	max(aa,bb) == if aa < bb then bb else aa;
}

--!! RemoveThis when the problem of unshared exports (in the 'add' body)
--!! from shared 'with' type forms is resolved.
IndexedBits(mn:SI): Join(BasicType, Aggregate(Boolean), Logic) with {
	RemoveThis: %;
	string:Literal -> %;
}
== add {
	Rep ==> Record(size: SI, nvalues:SI, values: BArr);

	import from Rep;
	import from Machine;

	RemoveThis: % == empty();
	sample: % == empty();	--!! Should get defaults from Aggregate(Boolean)

	apply(v:%, i:SI): Boolean ==
		get(BBool)(rep(v).values, (i-mn)::BSInt)::Boolean;
	set!(v:%, i:SI, val:Boolean): Boolean ==
		set!(BBool)(rep(v).values, (i-mn)::BSInt, val::BBool)::Boolean;

	new(n:SI, val:Boolean):% == {
		v := array(BBool)(false::BBool, n::BSInt);
		for i in 0..n-1 repeat
			set!(BBool)(v, i::BSInt, val::BBool);
		per [n, n, v];
	}
	extend!(u: %, val: Boolean): ()  == {
	        r := rep u;
	        if r.nvalues = r.size then {
	                nsize     := max(4, r.size + r.size quo 2);
	                v := array(BBool)(false::BBool, nsize::BSInt);
	                oldv := r.values;
	                for i in 0..r.nvalues-1 repeat
				set!(BBool)(v, i::BSInt,
						 get(BBool)(oldv, i::BSInt));
	                r.values  := v;
	                r.size    := nsize;
	        }
	        r.nvalues := r.nvalues+1;
	        set!(BBool)(r.values, (r.nvalues-1)::BSInt, val::BBool);
	}

	empty?(v:%):Boolean == #v=0;
	empty(n:SI):% == per [n, 0, array(BBool)(false::BBool, n::BSInt)];
	minIndex (u:%):SI == mn;
	range(v:%,i:SI):SI == {
		not (i < 0) and i < #v => i;
		error "Index out of range";
	}
--      coerce (v:%):OutputForm == {
--      	import from Character;
--      	import from String;
--      	import from StringCategory;
--      	t:Character := char "1";
--      	f:Character := char "0";
--      	s := new(#v,space()$Character)$String;
--      	for i in minIndex s..maxIndex s for j in mn.. repeat
--      		(s i := (v j => t; f));
--      	s::OutputForm;
--      }
	empty():% == new(0,false);
	copy (v:%):% == [vv for vv in v];
	#(v:%):SI == rep(v).nvalues;
	(v:%) ~= (u:%): Boolean == ~(v = u);
	(v:%) =  (u:%): Boolean ==  {
		#v ~= #u => false;
		for vv in v for uu in u repeat
			if vv ~= uu then return false;
		true;
	}
	(v:%) < (u:%): Boolean == {
		for vv in v for uu in u repeat
			uu ~= vv => {
				uu = false => return true;
				return false;
		       }
		#v < #u
	}
	(u:%) /\ (v:%):% == [uu and vv for uu in u for vv in v];
	(u:%) \/ (v:%):% == [uu or vv for uu in u for vv in v];
	xor(v:%,u:%):%   == [uu ~= vv for uu in u for vv in v];
	~(v:%):%         == [not vv for vv in v];

	generator(v: %): Generator Boolean == generate
	          for i in mn..#v+mn-1 repeat yield v.i;

	[g: Generator Boolean]: % == {
	          w := empty();
	          for t in g repeat extend!(w, t);
	          w;
	}
	[t: Tuple Boolean]: % == {
	          w := empty length t;
	          for i in 1..length t repeat extend!(w, element(t, i));
	          w;
	}
	map(f: Boolean->Boolean, u: %): %	      ==
		error "map$IndexedBits not implemented";
	map(f: (Boolean,Boolean)->Boolean, u: %, v: %): % ==
		error "map$IndexedBits not implemented";
	string(l:Literal):% == {
	          s: String := string l;
	          z: Character := char "0";
	          [ss ~= z for ss in s];
	}
	(p: TextWriter) << (v: %): TextWriter == {
	          q:String := "_"";
	          p << q;
	          for i in 1..#v repeat p << v.i;
	          p << q
	}
}


Bits: Join(BasicType, Aggregate(Boolean), Logic) with {
	string:Literal -> %;
}
== IndexedBits(1$SI) add;
