aldor -g loop -Mno-release <<ThatsAll
#int timing off
#include "axllib"
import from SingleInteger
34232423
import from List SingleInteger
a := [x for x in 1..50]
b := [2 * x for x in a]
c := [x * x for x in b]
import from Ratio
import from Ratio SingleInteger
43/223
import from List Ratio SingleInteger
k 
k := [[ x/y for x in c] for y in a]
k := [[ (x/y) for x in c] for y in a]
[ 1/2]
k := [ (x/y) for x in c for y in a]
first k
rest k
g1 = generator(-10..10)
g1 := generator(-10..10)
g1
for x in g1 repeat print << x
for x in g1 repeat print << x
print << newline
g1
GI==>Generator SingleInteger
I==>Integer
fact(x:I):I == if x < 2 then 1 else x * fact(x-1)
g := 1
u
u := [fact x for x in 50..1]
u := [fact x::Integer for x in 50..1]
u := [fact(x::Integer) for x in 50..1]
u := [fact(x::Integer) for x:SingleInteger in 50..1]
u := [fact(x::Integer) for x:SingleInteger in 50..1]
24
34
import from I
u := [fact(x::Integer) for x:SingleInteger in 50..1]
import from List I
u := [fact(x::Integer) for x:SingleInteger in 50..1]
u
u := [fact(x::Integer) for x:SingleInteger in 50..46 by -1]
3 * 1/2
k1 := [x * y for x in u for y in k]
k1 := [x * y for x in u for y in k]
import from Ratio Integer
k1 := [x * y for x in u for y in k]
234234434234243224
234234232344223@Integer
(234234232344223@Integer) * 234234324/3423423
u
k
k := [ (num x)::Integer / (denom x)::Integer for x in k]
k := [ (numer x)::Integer / (denom x)::Integer for x in k]
k := [ (numer x)::Integer / (denom x)::Integer for x in k]
k
k0 := [ (numer x)::Integer / (denom x)::Integer for x in k]
[ (31232131@Integer) / (4322423423@Integer)]
import from List Ratio Integer
k0 := [ (numer x)::Integer / (denom x)::Integer for x in k]
k := [ (numer x)::Integer / (denom x)::Integer for x in k0]
k
k2
k2 := [ (numer x)::Integer / (denom x)::Integer for x in k0]
k2 := [ (numer x)::Integer / (denom x)::Integer for x in k0]

#quit
ThatsAll
