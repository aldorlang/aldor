#include "algebra"
#include "aldorinterp"

macro P == IntegerPolynomial;
import from String, P, Integer;

x: P := "x" :: P;
y: P := "y" :: P;
z: P := "z" ::P;

p := (y + z)*x^2 + (y^3 + z)*x + z^4 + 3::P;


