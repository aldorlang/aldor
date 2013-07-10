#include "axllib.as"

define ItemCat: Category == with {	
	+: (%,%)->%;
	null: %;
	output: % -> ();
}

MySI: ItemCat with {
	bracket: SInt$Machine -> %;
	coerce: % -> SInt$Machine;
} == add {
	Rep ==> SInt$Machine;
	import from Rep, Machine;
	import from SingleInteger;

	bracket(i: SInt$Machine): % == per i;
	coerce(o: %): SInt$Machine == rep(o);
	
	(o1: %) + (o2: %): % == per (rep(o1) + rep(o2));
	null: % == per coerce(0$SingleInteger);
	output(o: %): () == print << rep(o)::SingleInteger;
}

MyString: ItemCat with {
	bracket: String -> %;
	coerce: % -> String;
} == add {
	Rep ==> String;
	import from Rep;

	bracket(s: String): % == per s;
	coerce(o: %): String == rep(o);
	
	(o1: %) + (o2: %): % == per (concat(rep(o1),rep(o2)));
	null: % == per "";
	output(o: %): () == print << rep(o);
}

MyList(I: ItemCat): with {
	bracket: () -> %;
	append:  (I,%) -> %;
	total:   % -> I;
	output:  % -> ();
} == add {
	Rep ==> Union(empty__list: Pointer, real__list: Record(i: I, next: %));
	import from Rep;

	bracket(): % == per [nil];
	append(i: I, o: %): % == 
		if (rep(o) case empty__list)
		then per [[i,per [nil]]];
		else per [[i,o]];
	total(o: %): I == {
		local tmp: % := o;
		local acc: I := null;
		while ( not (rep(tmp) case empty__list)) repeat {
			acc := acc + rep(tmp).real__list.i;
			tmp := rep(tmp).real__list.next;
		}
		acc
	}
	output(o: %): () == {
		local tmp: % := o;
		while ( not (rep(tmp) case empty__list)) repeat {
			print << "[";
			output(rep(tmp).real__list.i);
			tmp := rep(tmp).real__list.next;
			print << "]";
		}
		print << newline;
	}
}

runlist():() == {
	import from String, SInt$Machine, MySI, MyString, MyList(MyString), MyList(MySI),SingleInteger;
	ml__str: MyList(MyString) := append(["Hello,"],append([" I am "],append(["happy to "],append(["be there."],[]))));
	ml__si : MyList(MySI) := append([coerce(32)],append([coerce(54)],append([coerce(73)],[])));
	s: MyString := total(ml__str);
	i: MySI := total(ml__si);

	print << "List 1: "; output(ml__str); print << newline;
	print << "List 2: "; output(ml__si); print << newline;
	output(s); print << newline;
	output(i); print << newline;
}

