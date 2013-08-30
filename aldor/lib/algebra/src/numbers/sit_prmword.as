#include "algebra"

Z ==> MachineInteger;

WordSizedPrimes: with {
    prime?: Z -> Boolean;
} 
== add {

    prime?(p: Z): Boolean == {
	if p < 0 then p := -p;
	q := machine p;
	q < 2 => false;
	q = 2 => true;
	primeInCollection?(q)$SmallPrimes => true;
	primeInCollection?(q)$HalfWordSizePrimes => true;
	for i in 0..1000 repeat {
	   a := random(q-1) + 1;
	   assert a > 0;
	   assert a < q;
	   if mod_^(a, q-1, q) ~= -1 then return false;
	}
	return true;
    }
}

