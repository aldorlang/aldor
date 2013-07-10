#include "aldor"

AF ==> Array F;         SI ==> MachineInteger;

derivatives(F:Field, funvals:AF, h:F, rt3: F):(AF) == {
        import from SI;         n:SI   := # funvals;

        (n < 5) => error "Too few funct. values for boundary formulas";

        -- initialize local arrays
        b  :AF := new(n,0);     eta :AF := new(n,0); 
  	x  :AF := new(n,0);  	y   :AF := new(n,0);

	--*-*-*-* define integer<-->field operations *-*-*-*--
  	(n:SI) * (x:F):F == n::F * x;	(x:F) * (n:SI):F == n::F * x;
  	(n:SI) + (x:F):F == n::F + x;  	(x:F) + (n:SI):F == n::F + x;
  	(n:SI) - (x:F):F == n::F - x;  	(x:F) - (n:SI):F == x - n::F;
  	(n:SI) / (x:F):F == n::F / x;  	(x:F) / (n:SI):F == x/(n::F);
	--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*--

	c:F := 2::F + rt3;	cinv:= 2::F - rt3;  -- rt3 = sqrt(3)
	
	-- Prepare the right-hand side differences

	b(1) := (-(25*c+3)/12*funvals(1) + (24*c-5)/6*funvals(2)
		-3*(2*c-1@F)/2*funvals(3) + (8*c-3)/6*funvals(4)
		-(3*c-1@F)/12*funvals(5) )/h;
  
  	for i:SI in 2..(n-1) repeat b(i) := 3*(funvals(i+1) - funvals(i-1))/h;

	b(n) := (103*funvals(n) - 182*funvals(n-1) + 126*funvals(n-2)
        	- 58*funvals(n-3) + 11*funvals(n-4))/(12*h);

        -- Solve Ly = b

        y(1)   := b(1);
        for i:SI in 2..n repeat y(i) := b(i) - cinv * y(i-1);

        -- Solve Ux = y

        x(n) := cinv * y(n);
        for i:SI in (n-1)..1 by -1 repeat
                x(i) := cinv * (y(i) - x(i+1));

  	x
}
-- Add a square root function to DoubleFloat so 2 + sqrt(3) is ok.

extend DoubleFloat: with {sqrt: % -> %} == add {
	import from DoubleFloatElementaryFunctions;
	sqrt(x:%):% == (sqrt$DoubleFloatElementaryFunctions)(x);
}







