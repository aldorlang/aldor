-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen c  -Nsys=fortran-cmplx-void -laxllib
#include "axllib"

SI ==> SingleInteger;
SF ==> SingleFloat;
DF ==> DoubleFloat;
SC ==> Complex SF; -- FSComplex;
DC ==> Complex DF; -- FDComplex;
IA ==> Array SI; -- FSingleIntegerArray;
RA ==> Array SF; -- FSingleArray;
MDRA ==> Array Array SF; -- FMultiDimensionalArray(SingleFloat, RA);
-- MDRA ==> TwoDimensionalArray SF;
-- MDRA ==> FMultiDimensionalArray(SingleFloat, RA);
-- ARRDATA ==> FortranMultiDimensionalArrayData;

import from SF;
import from DF;


import {
	x: (a : SC, b : String, c : SI, d : BDFlo, 
            e : String, f : DC, g : IA, h : MDRA) -> BSFlo;
	y: (SC, String, SI, BDFlo, String, DC, IA, MDRA) -> BSFlo;
} from Foreign Fortran; 

export {
	xl: (a : SC, b : String, c : SI, d : BDFlo, 
             e : String, f : DC, g : IA, h : MDRA) -> BSFlo;
} to Foreign Fortran;

xl(a : SC, b : String, c : SI, d : BDFlo, e : String, f : DC, 
   g : IA, h : MDRA) : BSFlo ==
{
	x(a,b,c,d,e,f,g,h);
}

local {
	a : SC;
	a2 : SC;
	b : String;
	b2 : String; 
	c : SingleInteger;
	d : BDFlo;
	e : String; 
	f : DC;
	g : IA;
	g2 : IA;
	h : MDRA;
	h2 : MDRA;
	r : BSFlo;
	val : SingleInteger;
}

a := complex(1.5, 2.5);
a2 := a;

-- copy is needed to move String into non read only memory
b := copy "hello";
b2 := b;

c := 99;
d := 101.1 :: BDFlo;
e := copy "world";
f := complex(5.5, 6.5);

g := new(10, 1000);
g2 := g;


h := empty(2);
val := 1;
for i in 1..2 repeat
{
   extend!(h, empty(3));

   for j in 1..3 repeat
   {
      extend!(h(i), val :: SF);
      val := val + 1;
   }
}
h2 := h;

print << "real a = " << real a << newline;
print << "imag a = " << imag a << newline;
print << "real a2 = " << real a2 << newline;
print << "imag a2 = " << imag a2 << newline;  
print << "b = " << b << " b2 = " << b2 << newline;
print << "c = " << c << newline;
print << "d = " << d :: DoubleFloat << newline;
print << "e = " << e << newline;
print << "real f = " << real f << newline;
print << "imag f = " << imag f << newline;
print << "g(1) = " << g(1) << " g(10) = " << g(10) << newline;
print << "g2(1) = " << g2(1) << " g2(10) = " << g2(10) << newline;
print << "h[1,1] = " << h(1)(1) << newline;
print << "h[2,1] = " << h(2)(1) << newline;
print << "h[1,2] = " << h(1)(2) << newline;
print << "h[2,2] = " << h(2)(2) << newline;
print << "h[1,3] = " << h(1)(3) << newline;
print << "h[2,3] = " << h(2)(3) << newline;
print << "h2[1,1] = " << h2(1)(1) << newline;
print << "h2[1,2] = " << h2(1)(2) << newline;
print << "h2[2,3] = " << h2(2)(3) << newline << newline;

print << "Calling Fortran..." << newline << newline;

r := x(a,b,c,d,e,f,g,h);

-- a will be the new value returned from Fortran
print << "real a = " << real a << newline;
print << "imag a = " << imag a << newline;
print << "real a2 = " << real a2 << newline;
print << "imag a2 = " << imag a2 << newline;  
-- b and b2 are the same String modified by Fortran  
print << "b = " << b << " b2 = " << b2 << newline;
print << "c = " << c << newline;
print << "d = " << d :: DoubleFloat << newline;
print << "e = " << e << newline;
print << "real f = " << real f << newline;
print << "imag f = " << imag f << newline;
-- g and g2 are the same IA modified by Fortran 
print << "g(1) = " << g(1) << " g(10) = " << g(10) << newline;
print << "g2(1) = " << g2(1) << " g2(10) = " << g2(10) << newline;
-- h and h2 are the same MDRA modified by Fortran 
print << "h[1,1] = " << h(1)(1) << newline;
print << "h[2,1] = " << h(2)(1) << newline;
print << "h[1,2] = " << h(1)(2) << newline;
print << "h[2,2] = " << h(2)(2) << newline;
print << "h[1,3] = " << h(1)(3) << newline;
print << "h[2,3] = " << h(2)(3) << newline;
print << "h2[1,1] = " << h2(1)(1) << newline;
print << "h2[1,2] = " << h2(1)(2) << newline;
print << "h2[2,3] = " << h2(2)(3) << newline;
print << "r = " << r :: SingleFloat << newline << newline;

print << "Calling Fortran..." << newline << newline;

r := y(complex(1.5, 2.5), "hello", 99,
	101.1 :: BDFlo, "world", complex(5.5, 6.5),
	new(10, 1000), h);

