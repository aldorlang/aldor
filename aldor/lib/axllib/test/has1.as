-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
--> testcomp
--> testrun -l axllib

import from Integer;
#assert true


#if true
-- Has expressions
C3: Category ==  with { x: Integer };
C4: Category == C3 with { y: Integer };

S: with { foo: String } == add { foo: String == "a C3"; };

T(X: C3): with { foo: String } == S add {
	if (X has C4) then
		foo : String == "a C4";
}

U: C3 == add { x: Integer == 2}
V: C4 == U add { y: Integer == 2}
print << foo$T(U) << newline;
print << foo$T(V) << newline;
#endif

test(X: with { = : (%,%)->Boolean}, test: X, value: X, name: String):() == {
	print << name;
 	if (test = value) then 
		print <<" OK"<<newline;
	else 
		print <<" Failed"<<newline;
}
#if true
Test3():() == {
	test(Boolean, Array(Integer) has Aggregate(Integer), true, "T01");
	test(Boolean, Array(String) has Aggregate(Integer), false, "T02");
	test(Boolean, String has Aggregate(Integer), false, "T03");
};

Test3();

#endif

#if true
Test4(): () == {
	test(Boolean, Integer has Ring, true, "TX1");
	test(Boolean, Integer has EuclideanDomain, true, "TX2");
	test(Boolean, Integer has Field, false, "TX3");
	test(Boolean, Integer has Order, true, "TX4");
	test(Boolean, Integer has Join(), true, "T11");
	test(Boolean, Integer has Join(Ring), true, "T12");
	test(Boolean, Integer has Join(Ring, EuclideanDomain), true, "T13");
	test(Boolean, Integer has Join(Ring, EuclideanDomain, Order), true, "T14");
	test(Boolean, Integer has Join(Ring, Field, Order), false, "T15");
	test(Boolean, Integer has Join(Ring, EuclideanDomain, Field), false, "T16");
	test(Boolean, Integer has Join(Field, EuclideanDomain, Order), false, "T17");
}

Test4();
#endif
#if true
-- Has (w. simple with)

Test2(): () == {
	D1(D2: BasicType) : with { x: Integer } == add {
		x: Integer == if (D2 has with { rem: (%, %) -> %} ) then 12 else 4;
	}
	test(Integer,x$D1(Integer), 12, "TY1");
	test(Integer,x$D1(SingleFloat), 12, "TY2");
	test(Integer,x$D1(String), 4, "TY3");
}

Test2();

#endif

#if 0
-- Nastier examples
Test5(): () == {
	test(Boolean, (Integer has with {0:%} ), true, "T21");
	test(Boolean, (Character has with {space:%} ), true, "T22");
	test(Boolean, (Integer has with {<: (%,%)->Boolean} ), true, "T23");
	test(Boolean, (Character has Order with {space:%} ), true, "T24");
	test(Boolean, (Character has Join(Order, Finite) with {space:%} ), true, "T25");
	test(Boolean, (Character has Join(Order) with {Finite, space:%} ), true, "T26");
	test(Boolean, (Integer has Join(Order) with {Finite} ), false, "T27");
}

Test5();

#endif
