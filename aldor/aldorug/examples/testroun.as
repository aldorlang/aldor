#include "aldor"
--#assert broken

-- This is a double test.
-- 1) You need to say .ao in the #library command (confusion with .lib?)
-- 2) compile with aldor -Fx -laldor -M no-mactext testround.as RoundedRatio.ao
--    no-mactext means don't point at the macro definition when you find
--    an error in the text
-- 3) 1@R means `I know this is ambiguous but pick the 1 from R'.  It's 
--    better than 1::R because that might mean coerce the MachineInteger 1
--    to an R or coerce the Integer 1 to an R.

#library rrlib "rratio.ao"
#library compactlib "compact.ao"


SI ==> MachineInteger;
import from CommandLine, Array String;

strarg:String := if #arguments = 1 then arguments(1) else "10000";
k:SI == (retract) (scanNumber strarg);

F ==> RoundedRatio (SI, k); -- Errors should be less than 10^(-6)
SI==> MachineInteger; 	
AF==> Array F;

import from 	rrlib, compactlib, Integer, SI, F, AF, InFile, StandardIO, 
		NumberScanPackage Integer, String;

print << "Enter n : ";			
line: String := readline! stdin;
n:SI := (retract$Integer) (scanNumber line);
print << "n is " << n << newline; 	-- Good people echo their input.

h:F  := 1@F/(n::F);

x:AF   := new(n, 0);  -- x values
f:AF   := new(n, 0);  -- function values
xdf:AF := new(n, 0);  -- exact derivatives
adf:AF := new(n, 0);  -- approximate derivatives

fun(t:F):F == { 1@F/(1@F + t*t) };	
dfun(t:F):F == { -2*t/(1@F + t*t)/(1@F + t*t) };

for i:SI in 1..n repeat { 
  	x(i) := (i::F)*h ;
	f(i) := fun(x(i));
	xdf(i) := dfun(x(i));
}

rt3 := sqrt(3@SI::F);
adf := derivatives(F, f, h, rt3);

errors : AF := new(n,0);
for i:SI in 1..n repeat 	errors(i) := abs (xdf(i)/(h*h*h*h) - adf(i)/(h*h*h*h)) ;

-- The following bit of code is modelled on the list routine "reduce".
maxerror(e:AF):F == {
  	ans:F := 0;
  	for er in e repeat ans := max(ans,er);
  	ans
}
print << "The errors divided by h^4 are less than or equal to " << maxerror(errors) << newline;





