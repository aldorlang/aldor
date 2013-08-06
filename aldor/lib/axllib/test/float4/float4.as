-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testrun -Q3 -l axllib

#include "axllib"
#pile


FloatList==> [0.0,0.1,0.4,0.8,1.0,1.1,1.4,1.8,2.0,2.4,2.7,10.0,10.001,_
	      10.000002,30.2,200.32344,4e10,4e20,4e30, 888888888.99999999999]


import from String, Integer;
import from List RoundingMode, List Boolean;

strMode(r:RoundingMode):String ==
	r = zero() => "zero"
	r = nearest() => "nearest"
	r = down() => "down"
	r = up() => "up"
	r = any() => "any"
	error "Unknown mode"

test(F:BasicType, x:F):() ==
	if F has with {
		fraction: % -> %;
		integer: % -> Integer;
		round: (%, mode:RoundingMode == nearest()) -> Integer;
	}
	then
		import from F;
		print << "Original: " << x << newline;
		print << "integer " << x << " = " << integer x << newline;
		print << "fraction " << x << " = " << fraction x << newline;
		print << "round " << x << " = " << round x << newline;

		for mode in [nearest(), up(), down(), zero(), any()] repeat
			print << "round(" << x << "," << strMode(mode)
			print << ") = "
			print << round(x, mode) << newline;
	else
		error "Wrong parameter"

testFloatType(F:BasicType):() ==
		if F has with {
			float: Literal -> %;
			-: % -> %;
		}
		then 
			import from F, List F;
			for sign in [true, false] repeat
				for x in FloatList repeat
					if sign then
						test(F, x) 
					else 
						test(F, -x)


testSFloRange():() ==
	import from SingleFloat;
	x : SingleFloat := 0.0;

	for i in 1..100 repeat x := next x
	for i in 1..200 repeat x := prev x
	for i in 1..200 repeat x := next x
	for i in 1..100 repeat x := prev x

	if x ~= 0.0 then error "Wrong sflo prev/next"

testDFloRange():() ==
	import from DoubleFloat;
	x : DoubleFloat := 0.0;

	for i in 1..100 repeat x := next x
	for i in 1..200 repeat x := prev x
	for i in 1..200 repeat x := next x
	for i in 1..100 repeat x := prev x

	if x ~= 0.0 then error "Wrong dflo prev/next"



main():() ==
		import from Float
		nl ==> newline
		print << "---------- Testing SingleFloat ------------- " << nl
		testFloatType(SingleFloat) 
		print << "---------- Testing DoubleFloat ------------- " << nl
		testFloatType(DoubleFloat)
		print << "---------- Testing Float ------------- " << nl
		outputGeneral(-1)
		testFloatType(Float)
		print << "---------- Testing Float (fixed) ------------- " << nl
		outputFixed(-1)
		testFloatType(Float)
		print << "---------- Testing Float (float) ------------- " << nl
		outputFloating(-1)
		testFloatType(Float)
		print << "---------- Testing Float (general 7) ------------- " << nl
		outputGeneral(7)
		testFloatType(Float)
		print << "---------- Testing Float (fixed 7) ------------- " << nl
		outputFixed(7)
		testFloatType(Float)
		print << "---------- Testing Float (float 7) ------------- " << nl
		outputFloating(7)
		testFloatType(Float)
		print << "---------- Testing PFloat 20 ------------- " << nl
		testFloatType(PFloat 20)
		print << "---------- Testing PFloat 60 ------------- " << nl
		testFloatType(PFloat 60)
		
		testSFloRange()
		testDFloRange()


main()

