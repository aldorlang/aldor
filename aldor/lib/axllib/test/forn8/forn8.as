#include "axllib.as"

SI ==> SingleInteger;

define BasicItem: Category == with {
	display: % -> ();
	++ A comment!!
}

ItemComplex: BasicItem with {
	bracket: (SI,SI) -> %;
	plus   : (%,%) -> %;
} == add {
	Rep ==> Record(re: SI, im: SI);
	import from Rep;

	bracket(i: SI, j: SI): % == per [i,j];
	display(o: %): () == print << "Value of the complex: (" << rep(o).re << "," << rep(o).im << ")" << newline;
	plus(o1: %, o2: %): % == per [rep(o1).im + rep(o2).im, rep(o1).re + rep(o2).re];
}

ItemPrettyComplex: BasicItem with {
	bracket: (SI,SI,String) -> %;
	plus   : (%,%) -> %;
	getName: % -> String;
} ==  add {
	Rep ==> Record(base: ItemComplex, name: String);
	import from Rep;

	bracket(i: SI, j: SI, n: String): % == per [[i,j],n];
	plus(o1: %, o2: %): % == per [plus(rep(o1).base,rep(o2).base), concat(rep(o1).name," ",rep(o2).name)];
	getName(o: %): String == rep(o).name;
	display(o: %): () == {
		print << "Complex name: " << rep(o).name << newline;
		display(rep(o).base);
	}
}

DBComplexCat: Category == with {
	bracket: () -> %;
	insert:  (%,ItemComplex) -> ();
	display: % -> ();
}

DBComplex: DBComplexCat == add {
	Rep ==> Record(set: List ItemComplex);
	import from Rep;

	bracket(): % == per [nil];
	insert(o: %, i: ItemComplex): () == rep(o).set := cons(i,rep(o).set);
	display(o: %): () == {
		for i in tails rep(o).set repeat
			display(i.first);
	}
}
	

f(): () == {
	import from DBComplex, ItemComplex;
	import from SI;

	db: DBComplex := [];
	insert(db,[3,4]);
	insert(db,[12,4]);
	insert(db,[3,9]);
	insert(db,[0,1]);
	insert(db,[1,32]);
	insert(db,[3,45]);
	insert(db,[63,4]);
	display(db);
}
