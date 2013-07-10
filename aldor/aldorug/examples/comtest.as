-- compile with aldor -Fx -laldor -Q3 -M no-mactext comtest.as compact.ao

#include "aldor"
#library compactlib "compact.ao"
import from compactlib ;

SI==> MachineInteger; 	F ==> DoubleFloat;	AF==> Array F;

import from SI, F, AF, InFile, StandardIO, NumberScanPackage Integer, String;

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
dfun(t:F):F == { -2::F*t/(1@F + t*t)/(1@F + t*t) };

for i:SI in 1..n repeat { 
  	x(i) := (i::F)*h ;
	f(i) := fun(x(i));
	xdf(i) := dfun(x(i));
}

rt3:F := sqrt(3::F);
adf := derivatives(F, f, h, rt3);

errors : AF := new(n,0);
for i in 1..n repeat 	errors(i) := abs (xdf(i) - adf(i))/(h*h*h*h) ;

-- The following bit of code is modelled on the list routine "reduce".
maxerror(e:AF):F == {
  	ans:F := 0;
  	for er in e repeat ans := max(ans,er);
  	ans
}
print << "The errors divided by h^4 are less than or equal to " << maxerror(errors) << newline;



