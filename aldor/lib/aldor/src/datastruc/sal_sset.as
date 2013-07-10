----------------------------- sal_sset.as ------------------------------------
--
-- sortedset.as: A basic constructor for sorted sets based on sorted lists.
--
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
------------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{SortedSet}
\History{Marc Moreno Maza}{23/5/2001}{created}
\Usage{import from \this~T}
\Params{ {\em T} & \altype{PartiallyOrderedType} & the type of the entries\\ }
\Descr{\this(T) provides sorted sets whose entries belong 
to a partially ordered type. Duplicate entries are not allowed.}
\begin{exports}
\category{\altype{BoundedFiniteLinearStructureType} T}\\
\category{\altype{DynamicDataStructureType} T}\\
\end{exports}
#endif

SortedSet(T:PartiallyOrderedType):
	Join(BoundedFiniteLinearStructureType T, DynamicDataStructureType T) ==
		List T add {
	Rep == List(T);
	import from T, Rep;

	removeAll(t:T, x:%):%	== remove(t, x);
	removeAll!(t:T, x:%):%	== remove!(t, x);

	bracket(g:Generator T):% == {
		x: % := empty;
		for t in g repeat x := insert!(t, x);
		x;
	}

	bracket(t:Tuple T):% == {
		import from Z;
		x: % := empty;
		for i in 1..length t repeat x := insert!(element(t, i), x);
		x;
	}

	insert(t:T, x:%) : % == {
		empty? x => per cons(t, empty);
		l := rep x;
		t < first(l) => per cons(t, l);  
		t = first(l) => x;
		headNewl : Rep := [first(l)];
		newl : Rep := headNewl;  
		while (not empty?(l := rest l)) and not (t < first(l)) repeat {
			t = first(l) => return x;
			setRest!(headNewl,[first(l)]);
			headNewl := rest headNewl;
		}
		setRest!(headNewl,cons(t,l));
		per newl;
	}          

	local cons!(t:T, lt: Rep): Rep == {
		import from Boolean;
		assert(~empty? lt);
		l:Rep := [first lt];
		setRest!(l, rest lt);
		setRest!(lt, l);
		setFirst!(lt, t);
		lt;
	}

	insert!(t:T, x:%) : % == {
		import from Boolean;
		empty? x => per cons(t, empty);
		l := rep x;
		t < first(l) => per cons!(t,l);
		t = first(l) => x;
		m:Rep := empty;
		while (not empty?(l)) and not (t < first(l)) repeat {
			t = first(l) => return x;
			m := l;
			l := rest l;
		}
		assert(~empty? m);
		assert(empty?(l) or t ~= first l);
		setRest!(m,cons(t,l));
		x;
	}

	remove(t:T,x:%) : % == {
		import from Boolean;
		empty? x => x;
		l := rep x;
		t < first(l) => x;
		t = first(l) => per rest l;
		headNewl : Rep := [first(l)];
		newl : Rep := headNewl;  
		while (not empty?(l := rest l)) and not (t < first(l)) repeat {
			t = first(l) => break;
			setRest!(headNewl,[first(l)]);
			headNewl := rest headNewl;
		}
		if ((not empty? l) and (t = first(l))) then l := rest(l);
		setRest!(headNewl,l);
		per newl;
	}   

	remove!(t:T, x:%): % == {
		import from Boolean;
		empty? x => x;
		l := rep x;
		t < first(l) => x;
		t = first(l) => per rest l;
		m:Rep := empty;
		while (not empty?(l)) and not (t < first(l)) repeat {
			t = first(l) => {
				assert(~empty? m);
				setRest!(m, rest l);
				return x;
			}
			m := l;
			l := rest l;
		}
		x;
	}

	member?(t:T, x:%): Boolean == {
		empty? x => false;
		l := rep x;
		while ((not empty?(l)) and (t > first(l))) repeat l :=  rest l;
		while ((not empty?(l)) and (not(t < first(l)))) repeat {
			t = first(l) => return true;
			l :=  rest l;
		}
		false;
	}

	findAll(t:T, x:%): Generator Cross(Z, T) == generate {
		l := rep x;
		n:Z := 1;
		while ((not empty?(l)) and (t > first(l))) repeat {
			l :=  rest l;
			n := next n;
		}
		while ((not empty?(l)) and (not(t < first(l)))) repeat {
			if (t = first(l)) then {
				yield(n, t);
				l := empty;	-- l is a set, t appears once!
			} else {
				l :=  rest l;
				n := next n;
			}
		}
	}

	linearSearch(t:T, x:%, n:Z): (Boolean, Z, T) == {
		assert(n >= firstIndex);
		l := rep x;
		for i in 1..prev n repeat {
			empty? l => return (false, n, t);
			l := rest l;
		}
		empty? l => (false, n, t);
		while ((not empty?(l)) and (t > first(l))) repeat {
			l :=  rest l;
			n := next n;
		}
		while ((not empty?(l)) and (not(t < first(l)))) repeat {
			if (t = first(l)) then return (true, n, first(l));
			l :=  rest l;
			n := next n;
		}
		(false, n, t);
	}
}

