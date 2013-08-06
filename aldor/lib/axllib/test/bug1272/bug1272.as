--> testint -Q1

-- Original by Manuel Bronstein

-- % axiomxl -q2 -ginterp badcall.as
-- a = 2
-- b = 3
--
-- % axiomxl -q1 -ginterp badcall.as
-- a = 2
-- b = -1

#include "axllib"

define Foo: Category == with {
	foo: % -> (%, %, %);
	foo: (%, %) -> (%, %);
}


extend Integer: Foo == add {
	foo(x:%):(%, %, %) == {
		x < 0 => (-x, -1, -1);
		(x, 1, 1)
	}

	foo(x:%, z:%):(%, %) == {
		x < 0 => (-x, -z);
		(x, z)
	}
}

main():() == {
	import from Integer;
	(a, b) := foo(-2, -3);                  -- should be (2,3)
	print << "a = " << a << newline;
	print << "b = " << b << newline;
}

main()

