#include "aldor"

MINT ==> MachineInteger;

import {
  fsort: (Array(DoubleFloat),MINT,MINT,Ref(MINT)) -> ();
} from Foreign Fortran;

import {random : () -> Integer} from RandomNumberSource;
import from DoubleFloat;

-- Set up data
error?: MINT := 0;
n     : MINT := 10;
v     : Array(DoubleFloat) := new(10);
for i in 1..n repeat 
  set!(v,i,random()::DoubleFloat/random()::DoubleFloat);

fsort(v, 1, n, ref error?);

if zero? error? then {
  print << "sorted data: " << newline;
  for i in 1..n repeat print << v.(i::MINT) << newline;
}

